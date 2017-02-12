/*

Copyright 2010 by David A. Betz

This file is part of xmltar.

xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xmltar.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "Archive_Member.hpp"

std::string Archive_Member::Generate_Metadata(){
    DEBUGCXX(debugcxx,"Archive_Member::Generate_Metadata");
    std::string s;
    std::vector<std::pair<std::string,std::string> > attr_list;

    ssize_t keylist_size=llistxattr(file_path.string().c_str(),0,0);
    boost::scoped_array<char> xattr(new char[keylist_size]);

    ssize_t keylist_end=llistxattr(file_path.string().c_str(),xattr.get(),keylist_size);
    if (keylist_end==-1)
        throw "Archive_Member::Generate_Metadata: xattr list too small";

    for(unsigned int i=0; i<keylist_end; ++i){
        std::string key;
        for( ; i<keylist_end && xattr[i]!='\0'; ++i)
            key+=xattr[i];
        ssize_t val_size=lgetxattr(file_path.string().c_str(),key.c_str(),0,0);
        boost::scoped_array<char> xattr_val(new char[val_size]);
        if ((lgetxattr(file_path.string().c_str(),key.c_str(),xattr_val.get(),val_size))==-1)
            throw "Archive_Member::Generate_Metadata: xattr_val list too small";
        attr_list.push_back(std::pair<std::string,std::string>(key,std::string(xattr_val.get(),val_size)));
    };

    s=s+options_.Tabs("\t\t\t")+"<meta-data>"+options_.Newline();

    for(unsigned int i=0; i<attr_list.size(); ++i)
        s=s+options_.Tabs("\t\t\t\t")+"<extended-attribute key=\""
            +String_To_XML_Attribute_Value(attr_list[i].first)+"\" value=\""
            +String_To_XML_Attribute_Value(attr_list[i].second)+"\"/>"+options_.Newline();

    struct passwd *pw=getpwuid(stat_buf.st_uid);
    struct group *g=getgrgid(stat_buf.st_gid);

    s=s+options_.Tabs("\t\t\t\t")+"<mode value=\""+To_Octal_Int(stat_buf.st_mode)+"\"/>"+options_.Newline();
    s=s+options_.Tabs("\t\t\t\t")+"<atime posix=\"" + To_Decimal_Int(stat_buf.st_atime) + "\" localtime=\""+To_Local_Time(stat_buf.st_atime)+"\"/>"+options_.Newline();
    s=s+options_.Tabs("\t\t\t\t")+"<ctime posix=\"" + To_Decimal_Int(stat_buf.st_ctime) + "\" localtime=\""+To_Local_Time(stat_buf.st_ctime)+"\"/>"+options_.Newline();
    s=s+options_.Tabs("\t\t\t\t")+"<mtime posix=\"" + To_Decimal_Int(stat_buf.st_mtime) + "\" localtime=\""+To_Local_Time(stat_buf.st_mtime)+"\"/>"+options_.Newline();

    s=s+options_.Tabs("\t\t\t\t")+"<user uid=\""+To_Decimal_Int(stat_buf.st_uid)+"\" uname=\""+ (pw!=NULL?pw->pw_name:"") + "\"/>"+options_.Newline();
    s=s+options_.Tabs("\t\t\t\t")+"<group gid=\""+To_Decimal_Int(stat_buf.st_gid)+"\" gname=\""+ (g!=NULL?g->gr_name:"") + "\"/>"+options_.Newline();
    if (S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode)){
        s=s+options_.Tabs("\t\t\t\t")+"<rdev value=\""+To_Octal_Int(stat_buf.st_rdev)+"\"/>"+options_.Newline();
    }
    s=s+options_.Tabs("\t\t\t\t")+"<size value=\""+To_Decimal_Int(stat_buf.st_size)+"\"/>"+options_.Newline();

    s=s+options_.Tabs("\t\t\t")+"</meta-data>"+options_.Newline();

    return s;
}

std::string Archive_Member::Generate_Archive_Member_Header(){
    DEBUGCXX(debugcxx,"Archive_Member::Generate_Header");
    std::string s;

    s=s+options_.Tabs("\t\t")+"<file name=\"" + String_To_XML_Attribute_Value(file_path.relative_path().string()) + "\">"+options_.Newline();
    if (!metadata_written)
        s=s+       Generate_Metadata();

    switch(f_type){
        case boost::filesystem::regular_file:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"regular\">"+options_.Newline();
            s=s+options_.Tabs("\t\t\t\t")+"<stream name=\"data\" pre-compression=\"";
            if (options_.precompress==XmltarOptions::IDENTITY) s+="identity";
            else if (options_.precompress==XmltarOptions::GZIP) s+="gzip";
            else if (options_.precompress==XmltarOptions::BZIP2) s+="bzip2";
            else if (options_.precompress==XmltarOptions::XZ) s+="xz";
            else throw "unrecognized precompress";

            if (options_.encoding==XmltarOptions::BASE16) s+="\" encoding=\"base16";
            else if (options_.encoding==XmltarOptions::BASE64) s+="\" encoding=\"base64";
            else throw "unrecognized encoding";

            s+="\" total-size=\""+To_Decimal_Int(file_size)+"\" this-extent-start=\""+To_Decimal_Int(start_tell)+"\">"+options_.Newline();
            break;
        case boost::filesystem::directory_file:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"directory\"/>"+options_.Newline();
            break;
        case boost::filesystem::symlink_file:
        {
            s=s+options_.Tabs("\t\t\t")+"<content type=\"symlink\">"+options_.Newline();
            boost::scoped_array<char> p(new char[stat_buf.st_size]);
            if (readlink(file_path.string().c_str(),p.get(),stat_buf.st_size)!=stat_buf.st_size)
                throw "Archive_Member::Generate_Metadata: symbolic link size changed";
            s+=options_.Tabs("\t\t\t\t")+"<symlink target=\""+String_To_XML_Attribute_Value(std::string(p.get(),stat_buf.st_size))+"\"/>"+options_.Newline();
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

std::string Archive_Member::Generate_Archive_Member_Trailer(void){
    DEBUGCXX(debugcxx,"Archive_Member::Generate_Trailer");
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

Archive_Member::Archive_Member(boost::filesystem::path& fpath, const XmltarOptions& opt)
    :   options_(opt), file_path(fpath), start_tell(0), metadata_written(false),
        encoder(0), precompressor(0), archive_tb(0), is_attached(false), is_empty(false)
{
    DEBUGCXX(debugcxx,"Archive_Member::Archive_Member");

    f_stat=boost::filesystem::symlink_status(file_path);

    if (!boost::filesystem::exists(f_stat))
        throw "Archive_Member::Archive_Member: source file does not exist: "+file_path.string();

    f_type=f_stat.type();

    if (boost::filesystem::is_regular(f_stat))
        file_size=boost::filesystem::file_size(file_path);
    else file_size=0;

    if (lstat(file_path.string().c_str(),&stat_buf)!=0)
        throw "Archive_Member:Archive_Member: cannot lstat file";

    archive_member_header=Generate_Archive_Member_Header();
    archive_member_trailer=Generate_Archive_Member_Trailer();

    if (boost::filesystem::is_regular(f_stat)){
        ifs.open(fpath.string().c_str());

        if (!ifs)
            throw "Achive_Member::Archive_Member: cannot open input file";
    }

    encoder.reset(options_.New_Encoder());
    precompressor.reset(options_.New_Precompressor());
}

void Archive_Member::Attach(transitbuf_base *tb){
    archive_tb=tb;

    encoder->open(std::ios_base::out,tb);
    precompressor->open(std::ios_base::out,encoder.get());

    archive_member_header=Generate_Archive_Member_Header();

    is_attached=true;
}

void Archive_Member::Write(size_t n){
    DEBUGCXX(debugcxx,"Archive_Member::Write");

    if (n>file_size-start_tell) n=file_size-start_tell;

    if (!encoder->is_open())
        encoder->open(std::ios_base::out,archive_tb);
    if (!precompressor->is_open())
        precompressor->open(std::ios_base::out,encoder.get());
    char buf[1024];
    for(size_t i=n; ifs && i>0; ){
        ifs.read(buf,std::min(i,sizeof(buf)));;
        precompressor->sputn(buf,ifs.gcount());
        start_tell+=ifs.gcount();
        i-=ifs.gcount();
    }

    precompressor->pubsync();
    encoder->pubsync();
    archive_tb->pubsync();

    if (start_tell==file_size) is_empty=true;
}

size_t Archive_Member::Delta_Encoded_Length(size_t n, bool include_header){
    size_t result;

    if (boost::filesystem::is_regular(f_stat)){
        if (encoder.get()==0 || precompressor.get()==0){                        // TODO: when does Delta_Encoded_Length called with encoder.get()==precompressor.get()==0?
            throw "Archive_Member::Delta_Encoded_Length: encoder or preocompressor==0";
            boost::scoped_ptr<transitbuf_base> local_encoder(options_.New_Encoder());
            boost::scoped_ptr<transitbuf_base> local_precompressor(options_.New_Precompressor());
            std::ostringstream tmp;

            local_encoder->open(std::ios_base::out, tmp.rdbuf());
            local_precompressor->open(std::ios_base::out, local_encoder.get());

            result=local_encoder->encoded_length(local_precompressor->encoded_length(n));

            local_precompressor->close();
            local_encoder->close();
        }
        else
            result=encoder->encoded_length(precompressor->encoded_length(n));
    }
    else
        if (n!=0) throw "Archive_Member::Delta_Encoded_Length: Delta_Encoded_Length called on special file with n!=0";
        else result=archive_member_trailer.size();

    if (include_header) result+=archive_member_header.size();

    return result;
}
