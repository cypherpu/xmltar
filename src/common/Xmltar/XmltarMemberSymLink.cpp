/*
 * XmltarMemberSymLink.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dbetz
 */

#include <memory>

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <pwd.h>
#include <grp.h>
}
#include <boost/lexical_cast.hpp>

#include "Xmltar/XmltarMemberSymLink.hpp"
#include "Utilities/XmlEscapeAttribute.hpp"
#include "Utilities/CppStringEscape.hpp"
#include "Utilities/ToLocalTime.hpp"
#include "Utilities/ToDecimalInt.hpp"
#include "Utilities/ToOctalInt.hpp"
#include "Transform/TransformHex.hpp"
#include "../Debug2/Debug2.hpp"

XmltarMemberSymLink::XmltarMemberSymLink(XmltarOptions const & options, boost::filesystem::path const & filepath)
	: XmltarMember(options, filepath) {
	// betz::Debug dbg("XmltarMember::XmltarMember");

    f_stat=boost::filesystem::symlink_status(filepath_);

    if (!boost::filesystem::exists(f_stat))
        throw "Archive_Member::Archive_Member: source file does not exist: "+filepath_.string();

    f_type=f_stat.type();

    if (boost::filesystem::is_regular(f_stat))
        file_size=boost::filesystem::file_size(filepath_);
    else file_size=0;

    if (lstat(filepath_.string().c_str(),&stat_buf)!=0)
        throw "Archive_Member:Archive_Member: cannot lstat file";

    memberHeader_=MemberHeader();
    memberTrailer_=MemberTrailer();
}

void XmltarMemberSymLink::write(std::shared_ptr<Transform> archiveCompression, size_t committedBytes, size_t pendingBytes, std::ostream & ofs){
		betz::Debug2 dbg("XmltarMember::write");

		std::string tmp=MemberHeader()+MemberTrailer();
		std::string compressedDirectoryMember
			= options_.archiveMemberCompression_->CompressString(
					tmp
				);
		std::cerr << dbg << ": archiveCompression->QueuedWriteCount()=" << archiveCompression->QueuedWriteCount() << std::endl;
		archiveCompression->Write(compressedDirectoryMember);
		std::cerr << dbg << ": archiveCompression->QueuedWriteCount()=" << archiveCompression->QueuedWriteCount() << std::endl;

		isArchived_=true;
}

std::string XmltarMemberSymLink::MemberHeader(){
    std::string s=XmltarMember::MemberHeader();

	std::unique_ptr<char[]> p(new char[stat_buf.st_size]);
	if (readlink(filepath_.string().c_str(),p.get(),stat_buf.st_size)!=stat_buf.st_size)
		throw "Archive_Member::Generate_Metadata: symbolic link size changed";
	s+=options_.Tabs("\t\t\t")+"<content type=\"symlink\" target=\""+XmlEscapeAttribute(CppStringEscape(std::string(p.get(),stat_buf.st_size)))+"\"/>"+options_.Newline();

    return s;
}

std::string XmltarMemberSymLink::MemberTrailer(){
    std::string s;

    s=s+options_.Tabs("\t\t")+"</file>"+options_.Newline();

    return s;
}

std::string XmltarMemberSymLink::CompressedMemberHeader(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberHeader());
}

std::string XmltarMemberSymLink::CompressedMemberTrailer(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberTrailer());
}

size_t XmltarMemberSymLink::NumberOfFileBytesThatCanBeArchived(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
	betz::Debug2 dbg("XmltarMember::NumberOfFileBytesThatCanBeArchived");

	if (options_.tape_length_.get()<committedBytes+pendingBytes)
		throw std::logic_error("XmltarMember::NumberOfFileBytesThatCanBeArchived: overflow");

	size_t archiveBytes=options_.tape_length_.get()-committedBytes-pendingBytes;
	size_t uncompressedArchiveBytes=archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(archiveBytes);
	size_t uncompressedMemberBytes=options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(uncompressedArchiveBytes);
	size_t encodedMemberBytes;
	if ((memberHeader_.size()+memberTrailer_.size())>uncompressedMemberBytes)
		encodedMemberBytes=0;
	else
		encodedMemberBytes=options_.encoding_->MinimumPlaintextSizeGivenCompressedtextSize(uncompressedMemberBytes-memberHeader_.size()-memberTrailer_.size());
	size_t precompressedBytes=options_.fileCompression_->MinimumPlaintextSizeGivenCompressedtextSize(encodedMemberBytes);

	std::cerr << dbg << ": archiveBytes=" << archiveBytes << std::endl;
	std::cerr << dbg << ": uncompressedArchiveBytes=" << uncompressedArchiveBytes << std::endl;
	std::cerr << dbg << ": uncompressedMemberBytes=" << uncompressedMemberBytes << std::endl;
	std::cerr << dbg << ": encodedMemberBytes=" << encodedMemberBytes << " memberHeader_.size()=" << memberHeader_.size() << " memberTrailer_.size()=" << memberTrailer_.size() << std::endl;
	std::cerr << dbg << ": precompressedBytes=" << precompressedBytes << std::endl;

	return precompressedBytes;
}

bool XmltarMemberSymLink::CanArchive(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
	if (options_.tape_length_.get()<committedBytes+pendingBytes+memberHeader_.size()+memberTrailer_.size()) return false;

	size_t numberOfFileBytesThatCanBeArchived
		=	options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(
				archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(
					options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size()));

	return numberOfFileBytesThatCanBeArchived;
}

bool XmltarMemberSymLink::IsComplete(){
	return nextByte_==file_size;
}

boost::filesystem::path XmltarMemberSymLink::filepath(){
	return filepath_;
}

bool XmltarMemberSymLink::isDirectory(){
	return f_type==boost::filesystem::file_type::directory_file;
}

bool XmltarMemberSymLink::isSymLink(){
	return f_type==boost::filesystem::file_type::symlink_file;
}

bool XmltarMemberSymLink::isRegularFile(){
	return f_type==boost::filesystem::file_type::regular_file;
}

size_t XmltarMemberSymLink::NextByte(){
	return nextByte_;
}

void XmltarMemberSymLink::RecalculateMemberHeader(){
	memberHeader_=MemberHeader();
}

