/*
 * XmltarMember.cpp
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

#include "Xmltar/XmltarMember.hpp"
#include "Utilities/XmlEscapeAttribute.hpp"
#include "Utilities/CppStringEscape.hpp"
#include "Utilities/ToLocalTime.hpp"
#include "Utilities/ToDecimalInt.hpp"
#include "Utilities/ToOctalInt.hpp"
#include "Transform/TransformHex.hpp"
#include "../Debug2/Debug2.hpp"

XmltarMember::XmltarMember(XmltarOptions const & options, boost::filesystem::path const & filepath)
	: options_(options), filepath_(filepath), nextByte_(0), isArchived_(false) {
	// betz::Debug dbg("XmltarMember::XmltarMember");

    f_stat=boost::filesystem::symlink_status(filepath_);

    if (!boost::filesystem::exists(f_stat))
        throw "Archive_Member::Archive_Member: source file does not exist: "+filepath_.string();

    f_type=f_stat.type();

    if (boost::filesystem::is_regular(f_stat))
        file_size=boost::filesystem::file_size(filepath_);
    else file_size=0;

    if (lstat(filepath_.string().c_str(),&stat_buf_)!=0)
        throw "Archive_Member:Archive_Member: cannot lstat file";

    populateXAttrs();

    memberHeader_=commonHeader();
}

void XmltarMember::populateXAttrs(){
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
		if (val_size<=1)
			throw std::runtime_error("xattr empty");
		std::unique_ptr<char[]> xattr_val(new char[val_size]);
		if ((lgetxattr(filepath_.string().c_str(),key.c_str(),xattr_val.get(),val_size))==-1)
			throw "Archive_Member::Generate_Metadata: xattr_val list too small";

		xAttrs_.push_back({key,std::string(xattr_val.get(),val_size-1)});
	};
}

std::string XmltarMember::commonHeader(){
    std::string s;

    s=s+options_.Tabs("\t\t")+"<file name=\"" + XmlEscapeAttribute(CppStringEscape(filepath_.relative_path().string())) + "\">"+options_.Newline();

	s=s+options_.Tabs("\t\t\t")+"<meta-data>"+options_.Newline();

	for(unsigned int i=0; i<xAttrs_.size(); ++i)
		s=s+options_.Tabs("\t\t\t\t")+"<extended-attribute key=\""
			+XmlEscapeAttribute(CppStringEscape(xAttrs_[i].name_))+"\" value=\""
			+XmlEscapeAttribute(CppStringEscape(xAttrs_[i].value_))+"\"/>"+options_.Newline();

	struct passwd *pw=getpwuid(stat_buf_.st_uid);
	struct group *g=getgrgid(stat_buf_.st_gid);

	s=s+options_.Tabs("\t\t\t\t")+"<mode value=\""+ToOctalInt(stat_buf_.st_mode)+"\"/>"+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"<atime posix=\"" + std::to_string(stat_buf_.st_atime) + "\" localtime=\""+ToLocalTime(stat_buf_.st_atime)+"\"/>"+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"<ctime posix=\"" + std::to_string(stat_buf_.st_ctime) + "\" localtime=\""+ToLocalTime(stat_buf_.st_ctime)+"\"/>"+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"<mtime posix=\"" + std::to_string(stat_buf_.st_mtime) + "\" localtime=\""+ToLocalTime(stat_buf_.st_mtime)+"\"/>"+options_.Newline();

	s=s+options_.Tabs("\t\t\t\t")+"<user uid=\""+ToDecimalInt(stat_buf_.st_uid)+"\" uname=\""+ (pw!=NULL?pw->pw_name:"") + "\"/>"+options_.Newline();
	s=s+options_.Tabs("\t\t\t\t")+"<group gid=\""+ToDecimalInt(stat_buf_.st_gid)+"\" gname=\""+ (g!=NULL?g->gr_name:"") + "\"/>"+options_.Newline();
	if (S_ISCHR(stat_buf_.st_mode) || S_ISBLK(stat_buf_.st_mode)){
		s=s+options_.Tabs("\t\t\t\t")+"<rdev value=\""+ToOctalInt(stat_buf_.st_rdev)+"\"/>"+options_.Newline();
	}
	s=s+options_.Tabs("\t\t\t\t")+"<size value=\""+ToDecimalInt(stat_buf_.st_size)+"\"/>"+options_.Newline();

	s=s+options_.Tabs("\t\t\t")+"</meta-data>"+options_.Newline();

    return s;
}

std::string XmltarMember::MemberTrailer(){
    std::string s;

    // only include a content section if the file is a regular file
    if (f_type==boost::filesystem::regular_file){
        s=s+options_.Newline();
        s=s+options_.Tabs("\t\t\t\t")+"</stream>"+options_.Newline();
    }
    if (f_type==boost::filesystem::regular_file || f_type==boost::filesystem::symlink_file){
        s=s+options_.Tabs("\t\t\t")+"</content>"+options_.Newline();
    }
    s=s+options_.Tabs("\t\t")+"</file>"+options_.Newline();

    return s;
}
