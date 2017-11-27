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

#include "Xmltar/XmltarMember.hpp"
#include "Utilities/XMLEscapeAttribute.hpp"

XmltarMember::XmltarMember(XmltarOptions const & options, boost::filesystem::path const & filepath, bool includeMetadata)
	: options_(options), filepath_(filepath), includeMetadata_(includeMetadata) {

    f_stat=boost::filesystem::symlink_status(filepath_);

    if (!boost::filesystem::exists(f_stat))
        throw "Archive_Member::Archive_Member: source file does not exist: "+filepath_.string();

    f_type=f_stat.type();

    if (boost::filesystem::is_regular(f_stat))
        file_size=boost::filesystem::file_size(filepath_);
    else file_size=0;

    if (lstat(filepath_.string().c_str(),&stat_buf)!=0)
        throw "Archive_Member:Archive_Member: cannot lstat file";
}

std::string XmltarMember::Header(){
    std::string s;

    s=s+options_.Tabs("\t\t")+"<file name=\"" + XMLEscapeAttribute(filepath_.relative_path().string()) + "\">"+options_.Newline();
    if (!includeMetadata_){
        std::string s;
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
                +XMLEscapeAttribute(attr_list[i].first)+"\" value=\""
                +XMLEscapeAttribute(attr_list[i].second)+"\"/>"+options_.Newline();

        struct passwd *pw=getpwuid(stat_buf.st_uid);
        struct group *g=getgrgid(stat_buf.st_gid);

        s=s+options_.Tabs("\t\t\t\t")+"<mode value=\""+To_Octal_Int(stat_buf.st_mode)+"\"/>"+options_.Newline();
        s=s+options_.Tabs("\t\t\t\t")+"<atime posix=\"" + std::to_string(stat_buf.st_atime) + "\" localtime=\""+To_Local_Time(stat_buf.st_atime)+"\"/>"+options_.Newline();
        s=s+options_.Tabs("\t\t\t\t")+"<ctime posix=\"" + std::to_string(stat_buf.st_ctime) + "\" localtime=\""+To_Local_Time(stat_buf.st_ctime)+"\"/>"+options_.Newline();
        s=s+options_.Tabs("\t\t\t\t")+"<mtime posix=\"" + std::to_string(stat_buf.st_mtime) + "\" localtime=\""+To_Local_Time(stat_buf.st_mtime)+"\"/>"+options_.Newline();

        s=s+options_.Tabs("\t\t\t\t")+"<user uid=\""+To_Decimal_Int(stat_buf.st_uid)+"\" uname=\""+ (pw!=NULL?pw->pw_name:"") + "\"/>"+options_.Newline();
        s=s+options_.Tabs("\t\t\t\t")+"<group gid=\""+To_Decimal_Int(stat_buf.st_gid)+"\" gname=\""+ (g!=NULL?g->gr_name:"") + "\"/>"+options_.Newline();
        if (S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode)){
            s=s+options_.Tabs("\t\t\t\t")+"<rdev value=\""+To_Octal_Int(stat_buf.st_rdev)+"\"/>"+options_.Newline();
        }
        s=s+options_.Tabs("\t\t\t\t")+"<size value=\""+To_Decimal_Int(stat_buf.st_size)+"\"/>"+options_.Newline();

        s=s+options_.Tabs("\t\t\t")+"</meta-data>"+options_.Newline();
    }

    switch(f_type){
        case boost::filesystem::regular_file:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"regular\">"+options_.Newline();
            s=s+options_.Tabs("\t\t\t\t")+"<stream name=\"data\" pre-compression=\""+CompressionName(options_.fileCompression_.get());

            if (options_.encoding_==XmltarOptions::BASE16) s+="\" encoding=\"base16";
            else if (options_.encoding_==XmltarOptions::BASE64) s+="\" encoding=\"base64";
            else throw "unrecognized encoding";

            s+="\" total-size=\""+std::to_string(file_size)+"\" this-extent-start=\""+std::to_string(start_tell)+"\">"+options_.Newline();
            break;
        case boost::filesystem::directory_file:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"directory\"/>"+options_.Newline();
            break;
        case boost::filesystem::symlink_file:
        {
            s=s+options_.Tabs("\t\t\t")+"<content type=\"symlink\">"+options_.Newline();
            std::unique_ptr<char[]> p(new char[stat_buf.st_size]);
            if (readlink(filepath_.string().c_str(),p.get(),stat_buf.st_size)!=stat_buf.st_size)
                throw "Archive_Member::Generate_Metadata: symbolic link size changed";
            s+=options_.Tabs("\t\t\t\t")+"<symlink target=\""+XMLEscapeAttribute(std::string(p.get(),stat_buf.st_size))+"\"/>"+options_.Newline();
        }
            break;
        case boost::filesystem::block_file:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"block\"/>"+options_.Newline();
            break;
        case boost::filesystem::character_file:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"character\"/>"+options_.Newline();
            break;
        case boost::filesystem::fifo_file:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"fifo\"/>"+options_.Newline();
            break;
        case boost::filesystem::socket_file:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"socket\"/>"+options_.Newline();
            break;
        case boost::filesystem::type_unknown:
        case boost::filesystem::status_unknown:
        case boost::filesystem::file_not_found:
        default:
            throw "Archive_Member::Generate_Header: unable to stat file";
            break;
    }

    return s;
}

std::string XmltarMember::Trailer(){
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
