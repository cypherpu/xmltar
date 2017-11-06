/*
 * Ext4.cpp
 *
 *  Created on: Nov 4, 2017
 *      Author: dbetz
 */

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <pwd.h>
#include <grp.h>
}

#include "Metadata/Ext4.hpp"
#include "Include/Tar_Options.hpp"

 std::string Ext4::toString(XmltarOptions & options){
	std::string s;
    std::vector<std::pair<std::string,std::string> > attr_list;

    ssize_t keylist_size=llistxattr(path_.string().c_str(),0,0);
    boost::scoped_array<char> xattr(new char[keylist_size]);

    ssize_t keylist_end=llistxattr(path_.string().c_str(),xattr.get(),keylist_size);
    if (keylist_end==-1)
        throw "Archive_Member::Generate_Metadata: xattr list too small";

    for(unsigned int i=0; i<keylist_end; ++i){
        std::string key;
        for( ; i<keylist_end && xattr[i]!='\0'; ++i)
            key+=xattr[i];
        ssize_t val_size=lgetxattr(path_.string().c_str(),key.c_str(),0,0);
        boost::scoped_array<char> xattr_val(new char[val_size]);
        if ((lgetxattr(path_.string().c_str(),key.c_str(),xattr_val.get(),val_size))==-1)
            throw "Archive_Member::Generate_Metadata: xattr_val list too small";
        attr_list.push_back(std::pair<std::string,std::string>(key,std::string(xattr_val.get(),val_size)));
    };

    s=s+options.Tabs("\t\t\t")+"<meta-data>"+options.Newline();

    for(unsigned int i=0; i<attr_list.size(); ++i)
        s=s+options.Tabs("\t\t\t\t")+"<extended-attribute key=\""
            +String_To_XML_Attribute_Value(attr_list[i].first)+"\" value=\""
            +String_To_XML_Attribute_Value(attr_list[i].second)+"\"/>"+options.Newline();

    struct passwd *pw=getpwuid(stat_buf.st_uid);
    struct group *g=getgrgid(stat_buf.st_gid);

    s=s+options.Tabs("\t\t\t\t")+"<mode value=\""+To_Octal_Int(stat_buf.st_mode)+"\"/>"+options_.Newline();
    s=s+options.Tabs("\t\t\t\t")+"<atime posix=\"" + To_Decimal_Int(stat_buf.st_atime) + "\" localtime=\""+To_Local_Time(stat_buf.st_atime)+"\"/>"+options.Newline();
    s=s+options.Tabs("\t\t\t\t")+"<ctime posix=\"" + To_Decimal_Int(stat_buf.st_ctime) + "\" localtime=\""+To_Local_Time(stat_buf.st_ctime)+"\"/>"+options.Newline();
    s=s+options.Tabs("\t\t\t\t")+"<mtime posix=\"" + To_Decimal_Int(stat_buf.st_mtime) + "\" localtime=\""+To_Local_Time(stat_buf.st_mtime)+"\"/>"+options.Newline();

    s=s+options.Tabs("\t\t\t\t")+"<user uid=\""+To_Decimal_Int(stat_buf.st_uid)+"\" uname=\""+ (pw!=NULL?pw->pw_name:"") + "\"/>"+options.Newline();
    s=s+options.Tabs("\t\t\t\t")+"<group gid=\""+To_Decimal_Int(stat_buf.st_gid)+"\" gname=\""+ (g!=NULL?g->gr_name:"") + "\"/>"+options.Newline();
    if (S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode)){
        s=s+options.Tabs("\t\t\t\t")+"<rdev value=\""+To_Octal_Int(stat_buf.st_rdev)+"\"/>"+options.Newline();
    }
    s=s+options.Tabs("\t\t\t\t")+"<size value=\""+To_Decimal_Int(stat_buf.st_size)+"\"/>"+options.Newline();

    s=s+options.Tabs("\t\t\t")+"</meta-data>"+options.Newline();

    return s;
}
