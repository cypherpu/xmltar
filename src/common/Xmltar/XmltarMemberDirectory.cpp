/*
 * XmltarMemberDirectory.cpp
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

#include "Xmltar/XmltarMemberDirectory.hpp"
#include "Utilities/XmlEscapeAttribute.hpp"
#include "Utilities/CppStringEscape.hpp"
#include "Utilities/ToLocalTime.hpp"
#include "Utilities/ToDecimalInt.hpp"
#include "Utilities/ToOctalInt.hpp"
#include "Transform/TransformHex.hpp"
#include "../Debug2/Debug2.hpp"

XmltarMemberDirectory::XmltarMemberDirectory(XmltarOptions const & options, boost::filesystem::path const & filepath)
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

bool XmltarMemberDirectory::completed(){
	return nextByte_>=file_size;
}

void XmltarMemberDirectory::write(std::shared_ptr<Transform> archiveCompression, size_t committedBytes, size_t pendingBytes, std::ostream & ofs){
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

std::string XmltarMemberDirectory::MemberHeader(){
    std::string s;

    s=s+options_.Tabs("\t\t")+"<file name=\"" + XmlEscapeAttribute(CppStringEscape(filepath_.relative_path().string())) + "\">"+options_.Newline();

	std::vector<std::pair<std::string,std::string> > attr_list;

	ssize_t keylist_size=llistxattr(filepath_.string().c_str(),0,0);
	std::unique_ptr<char[]> xattr(new char[keylist_size]);

	ssize_t keylist_end=llistxattr(filepath_.string().c_str(),xattr.get(),keylist_size);
	if (keylist_end==-1)
		throw "Archive_Member::Generate_Metadata: xattr list too small";

	for(unsigned int i=0; i<keylist_end; ++i){
		std::string key;
		for( ; i<keylist_end && xattr[i]!='\0'; ++i)
			key+=xattr[i];
		ssize_t val_size=lgetxattr(filepath_.string().c_str(),key.c_str(),0,0);
		std::unique_ptr<char[]> xattr_val(new char[val_size]);
		if ((lgetxattr(filepath_.string().c_str(),key.c_str(),xattr_val.get(),val_size))==-1)
			throw "Archive_Member::Generate_Metadata: xattr_val list too small";
		attr_list.push_back(std::pair<std::string,std::string>(key,std::string(xattr_val.get(),val_size)));
	};

	s=s+options_.Tabs("\t\t\t")+"<meta-data>"+options_.Newline();

	for(unsigned int i=0; i<attr_list.size(); ++i)
		s=s+options_.Tabs("\t\t\t\t")+"<extended-attribute key=\""
			+XmlEscapeAttribute(CppStringEscape(attr_list[i].first))+"\" value=\""
			+XmlEscapeAttribute(CppStringEscape(attr_list[i].second))+"\"/>"+options_.Newline();

	struct passwd *pw=getpwuid(stat_buf.st_uid);
	struct group *g=getgrgid(stat_buf.st_gid);

	s=s+options_.Tabs("\t\t\t\t")+"<mode value=\""+ToOctalInt(stat_buf.st_mode)+"\"/>"+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"<atime posix=\"" + std::to_string(stat_buf.st_atime) + "\" localtime=\""+ToLocalTime(stat_buf.st_atime)+"\"/>"+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"<ctime posix=\"" + std::to_string(stat_buf.st_ctime) + "\" localtime=\""+ToLocalTime(stat_buf.st_ctime)+"\"/>"+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"<mtime posix=\"" + std::to_string(stat_buf.st_mtime) + "\" localtime=\""+ToLocalTime(stat_buf.st_mtime)+"\"/>"+options_.Newline();

	s=s+options_.Tabs("\t\t\t\t")+"<user uid=\""+ToDecimalInt(stat_buf.st_uid)+"\" uname=\""+ (pw!=NULL?pw->pw_name:"") + "\"/>"+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"<group gid=\""+ToDecimalInt(stat_buf.st_gid)+"\" gname=\""+ (g!=NULL?g->gr_name:"") + "\"/>"+options_.Newline();
	if (S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode)){
		s=s+options_.Tabs("\t\t\t\t")+"<rdev value=\""+ToOctalInt(stat_buf.st_rdev)+"\"/>"+options_.Newline();
	}
	s=s+options_.Tabs("\t\t\t\t")+"<size value=\""+ToDecimalInt(stat_buf.st_size)+"\"/>"+options_.Newline();

	s=s+options_.Tabs("\t\t\t")+"</meta-data>"+options_.Newline();

    s=s+options_.Tabs("\t\t\t")+"<content type=\"directory\"/>"+options_.Newline();

    return s;
}

std::string XmltarMemberDirectory::MemberTrailer(){
    std::string s;

    s=s+options_.Tabs("\t\t")+"</file>"+options_.Newline();

    return s;
}

std::string XmltarMemberDirectory::CompressedMemberHeader(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberHeader());
}

std::string XmltarMemberDirectory::CompressedMemberTrailer(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberTrailer());
}

size_t XmltarMemberDirectory::NumberOfFileBytesThatCanBeArchived(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
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

bool XmltarMemberDirectory::CanArchive(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
	if (options_.tape_length_.get()<committedBytes+pendingBytes+memberHeader_.size()+memberTrailer_.size()) return false;

	size_t numberOfFileBytesThatCanBeArchived
		=	options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(
				archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(
					options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size()));

	return numberOfFileBytesThatCanBeArchived;
}

bool XmltarMemberDirectory::IsComplete(){
	return nextByte_==file_size;
}

boost::filesystem::path XmltarMemberDirectory::filepath(){
	return filepath_;
}

bool XmltarMemberDirectory::isDirectory(){
	return f_type==boost::filesystem::file_type::directory_file;
}

bool XmltarMemberDirectory::isSymLink(){
	return f_type==boost::filesystem::file_type::symlink_file;
}

bool XmltarMemberDirectory::isRegularFile(){
	return f_type==boost::filesystem::file_type::regular_file;
}

size_t XmltarMemberDirectory::NextByte(){
	return nextByte_;
}

void XmltarMemberDirectory::RecalculateMemberHeader(){
	memberHeader_=MemberHeader();
}

