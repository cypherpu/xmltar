/*
 * XmltarMemberRegularFile.cpp
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

#include "Xmltar/XmltarMemberRegularFile.hpp"
#include "Utilities/XmlEscapeAttribute.hpp"
#include "Utilities/CppStringEscape.hpp"
#include "Utilities/ToLocalTime.hpp"
#include "Utilities/ToDecimalInt.hpp"
#include "Utilities/ToOctalInt.hpp"
#include "Transform/TransformHex.hpp"
#include "../Debug2/Debug2.hpp"

XmltarMemberRegularFile::XmltarMemberRegularFile(XmltarOptions const & options, boost::filesystem::path const & filepath)
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

void XmltarMemberRegularFile::write(std::shared_ptr<Transform> archiveCompression, size_t committedBytes, size_t pendingBytes, std::ostream & ofs){
		betz::Debug2 dbg("XmltarMember::write");

		size_t numberOfFileBytesThatCanBeArchived=NumberOfFileBytesThatCanBeArchived(committedBytes,pendingBytes,archiveCompression);

		std::ifstream ifs(filepath_.string());
		ifs.seekg(nextByte_);

		std::shared_ptr<Transform> precompression(options_.fileCompression_->clone());
		std::shared_ptr<Transform> memberCompression(options_.archiveMemberCompression_->clone());
		std::shared_ptr<Transform> encoding(options_.encoding_->clone());

		size_t numberOfBytesToArchive=std::min(file_size-nextByte_,(size_t)numberOfFileBytesThatCanBeArchived);
		precompression->OpenCompression();
		encoding->OpenCompression();
		memberCompression->OpenCompression();
		char buf[1024];

		std::cerr << dbg << ": memberHeader_=" << memberHeader_.size() << std::endl;
		memberCompression->Write(memberHeader_);

		std::string encoded, tmp;
		for( size_t i=numberOfBytesToArchive; ifs && i>0; i-=ifs.gcount(),nextByte_+=ifs.gcount()){
			ifs.read(buf,std::min((size_t)i,sizeof(buf)));
			//std::cerr << dbg << ": read " << ifs.gcount() << " bytes" << std::endl;
			precompression->Write(std::string(buf,ifs.gcount()));
			encoding->Write(precompression->Read());
			tmp=encoding->Read();
			encoded+=tmp;
			//std::cerr << "tmp=" << tmp << std::endl;
			memberCompression->Write(tmp);
			archiveCompression->Write(memberCompression->Read());
			ofs << archiveCompression->Read();
		}

		std::cerr << dbg << ": finished loop" << std::endl;
		encoding->Write(precompression->Close());
		std::cerr << dbg << ": 1" << std::endl;
		tmp=encoding->Close();
		std::cerr << dbg << ": 2" << std::endl;
		encoded+=tmp;
		memberCompression->Write(tmp);
		std::cerr << dbg << ": 3" << std::endl;
		memberCompression->Write(memberTrailer_);
		std::cerr << dbg << ": 4" << std::endl;
		std::string tmp2=memberCompression->Close();
		std::cerr << dbg << ": 4.5" << std::endl;
		archiveCompression->Write(tmp);
		std::cerr << dbg << ": 5" << std::endl;

		// std::cerr << tmp << std::endl;
		std::cerr << dbg << ": precompression->ReadCount=" << precompression->ReadCount() << std::endl;
		std::cerr << dbg << ": 6" << std::endl;
		std::cerr << dbg << ": precompression->WriteCount=" << precompression->WriteCount() << std::endl;
		std::cerr << dbg << ": encoding->ReadCount=" << encoding->ReadCount() << std::endl;
		std::cerr << dbg << ": encoding->WriteCount=" << encoding->WriteCount() << std::endl;
		std::cerr << dbg << ": memberCompression->ReadCount=" << memberCompression->ReadCount() << std::endl;
		std::cerr << dbg << ": memberCompression->WriteCount=" << memberCompression->WriteCount() << std::endl;

		if (nextByte_>=file_size)
			isArchived_=true;
}

std::string XmltarMemberRegularFile::MemberHeader(){
    std::string s=XmltarMember::MemberHeader();

	s=s+options_.Tabs("\t\t\t")+"<content type=\"regular\">"+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"<stream name=\"data\" pre-compression=\""+options_.fileCompression_.get()->CompressionName();

	s+=std::string("\" encoding=\"") + options_.encoding_.get()->CompressionName();

	s+="\" total-size=\""+std::to_string(file_size)+"\" this-extent-start=\""+std::to_string(nextByte_)+"\">"+options_.Newline();

    return s;
}

std::string XmltarMemberRegularFile::MemberTrailer(){
    std::string s;

	s=s+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"</stream>"+options_.Newline();
    s=s+options_.Tabs("\t\t\t")+"</content>"+options_.Newline();
    s=s+options_.Tabs("\t\t")+"</file>"+options_.Newline();

    return s;
}

std::string XmltarMemberRegularFile::CompressedMemberHeader(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberHeader());
}

std::string XmltarMemberRegularFile::CompressedMemberTrailer(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberTrailer());
}

size_t XmltarMemberRegularFile::NumberOfFileBytesThatCanBeArchived(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
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

bool XmltarMemberRegularFile::CanArchive(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
	return NumberOfFileBytesThatCanBeArchived(committedBytes,pendingBytes,archiveCompression)!=0;
}

bool XmltarMemberRegularFile::IsComplete(){
	return nextByte_==file_size;
}

boost::filesystem::path XmltarMemberRegularFile::filepath(){
	return filepath_;
}

bool XmltarMemberRegularFile::isDirectory(){
	return f_type==boost::filesystem::file_type::directory_file;
}

bool XmltarMemberRegularFile::isSymLink(){
	return f_type==boost::filesystem::file_type::symlink_file;
}

bool XmltarMemberRegularFile::isRegularFile(){
	return f_type==boost::filesystem::file_type::regular_file;
}

size_t XmltarMemberRegularFile::NextByte(){
	return nextByte_;
}

void XmltarMemberRegularFile::RecalculateMemberHeader(){
	memberHeader_=MemberHeader();
}

