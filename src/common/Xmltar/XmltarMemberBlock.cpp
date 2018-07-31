/*
 * XmltarMemberBlock.cpp
 *
 *  Created on: Jul 30, 2018
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

#include "Xmltar/XmltarMemberBlock.hpp"
#include "Utilities/XmlEscapeAttribute.hpp"
#include "Utilities/CppStringEscape.hpp"
#include "Utilities/ToLocalTime.hpp"
#include "Utilities/ToDecimalInt.hpp"
#include "Utilities/ToOctalInt.hpp"
#include "Transform/TransformHex.hpp"
#include "../Debug2/Debug2.hpp"

XmltarMemberBlock::XmltarMemberBlock(XmltarOptions const & options, boost::filesystem::path const & filepath)
	: XmltarMember(options, filepath) {
    memberTrailer_=MemberTrailer();
}

void XmltarMemberBlock::write(std::shared_ptr<Transform> archiveCompression, size_t committedBytes, size_t pendingBytes, std::ostream & ofs){
		betz::Debug2 dbg("XmltarMember::write");

		std::string tmp=MemberHeader()+MemberTrailer();
		std::string compressedDirectoryMember
			= options_.archiveMemberCompression_->CompressString(
					tmp
				);
		std::cerr << dbg << ": archiveCompression->QueuedWriteCount()=" << archiveCompression->QueuedWriteCount() << std::endl;
		archiveCompression->Write(compressedDirectoryMember);

		isArchived_=true;
}

std::string XmltarMemberBlock::MemberHeader(){
    std::string s=XmltarMember::commonHeader();
    s=s+options_.Tabs("\t\t\t")+"<content type=\"block\" rdev=\""+ToDecimalInt(stat_buf_.st_rdev)+"\"/>"+options_.Newline();

    return s;
}

std::string XmltarMemberBlock::MemberTrailer(){
    std::string s;

    s=s+options_.Tabs("\t\t")+"</file>"+options_.Newline();

    return s;
}

bool XmltarMemberBlock::CanArchive(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
	if (options_.tape_length_.get()<committedBytes+pendingBytes+memberHeader_.size()+memberTrailer_.size()) return false;

	size_t numberOfFileBytesThatCanBeArchived
		=	options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(
				archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(
					options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size()));

	return numberOfFileBytesThatCanBeArchived;
}
