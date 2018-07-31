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
    std::string s=XmltarMember::commonHeader();

	std::unique_ptr<char[]> p(new char[stat_buf_.st_size]);
	if (readlink(filepath_.string().c_str(),p.get(),stat_buf_.st_size)!=stat_buf_.st_size)
		throw "Archive_Member::Generate_Metadata: symbolic link size changed";
	s+=options_.Tabs("\t\t\t")+"<content type=\"symlink\" target=\""+XmlEscapeAttribute(CppStringEscape(std::string(p.get(),stat_buf_.st_size)))+"\"/>"+options_.Newline();

    return s;
}

std::string XmltarMemberSymLink::MemberTrailer(){
    std::string s;

    s=s+options_.Tabs("\t\t")+"</file>"+options_.Newline();

    return s;
}

bool XmltarMemberSymLink::CanArchive(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
	if (options_.tape_length_.get()<committedBytes+pendingBytes+memberHeader_.size()+memberTrailer_.size()) return false;

	size_t numberOfFileBytesThatCanBeArchived
		=	options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(
				archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(
					options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size()));

	return numberOfFileBytesThatCanBeArchived;
}
