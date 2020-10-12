/*

XmltarMember.cpp
Copyright 2017-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: Nov 26, 2017
Author: dbetz

This file is part of Xmltar.

Xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Xmltar.  If not, see <https://www.gnu.org/licenses/>.

 */

#include <memory>
#include <iomanip>
#include <fstream>

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <pwd.h>
#include <grp.h>
}
#include <boost/lexical_cast.hpp>
#include <spdlog/spdlog.h>

#include "Xmltar/XmltarGlobals.hpp"
#include "Xmltar/XmltarMemberCreate.hpp"
#include "Utilities/XMLSafeString.hpp"
#include "Utilities/ToLocalTime.hpp"
#include "Utilities/ToDecimalInt.hpp"
#include "Utilities/ToOctalInt.hpp"
#include "Utilities/PrintOpenFileDescriptors.hpp"
#include "Generated/Utilities/Debug2.hpp"

XmltarMemberCreate::XmltarMemberCreate(XmltarGlobals & globals, std::filesystem::path const & filepath)
	: globals_(globals), filepath_(filepath), metadataWritten_(false) {
    f_stat_=std::filesystem::symlink_status(filepath_);

    if (!std::filesystem::exists(f_stat_))
        throw "Archive_Member::Archive_Member: source file does not exist: "+filepath_.string();

    f_type_=f_stat_.type();

    if (std::filesystem::is_regular_file(f_stat_)){
        file_size_=std::filesystem::file_size(filepath_);
        ifs_.reset(new std::ifstream(filepath_.string()));
        sha3sum512_.Open();
        if (!*ifs_){
        	globals_.resultCode_=1;
        	globals_.errorMessages_.push_back(filepath_.string()+": cannot open file");
        }
    }
    else file_size_=0;

    if (lstat(filepath_.string().c_str(),&stat_buf_)!=0)
        throw "Archive_Member:Archive_Member: cannot lstat file";

    memberHeader_=MemberHeader();
    memberTrailer_=MemberTrailer();

    startingVolumeName_=globals_.current_xmltar_file_name_;
}

XmltarMemberCreate::~XmltarMemberCreate(){
}

void XmltarMemberCreate::write(size_t numberOfFileBytesThatCanBeArchived, std::ostream & ofs){
		size_t numberOfBytesToArchive=std::min(file_size_-ifs_->tellg(),(off_t)numberOfFileBytesThatCanBeArchived);

		ofs <<	globals_.options_.archiveEncryption_->Encrypt(
					globals_.options_.archiveCompression_->ForceWrite(
						globals_.options_.encoding_->Open(
							globals_.options_.fileCompression_->Open()
						)
					)
				);

		char buf[1024];

		ofs << globals_.options_.archiveEncryption_->Encrypt(
					globals_.options_.archiveCompression_->ForceWrite(memberHeader_))
				;
		metadataWritten_=true;
	    memberHeader_=MemberHeader();

		for( size_t i=numberOfBytesToArchive; *ifs_ && i>0; i-=ifs_->gcount()){
			ifs_->read(buf,std::min((size_t)i,sizeof(buf)));
			if (globals_.options_.sha3_512_)
				sha3sum512_.ForceWrite(std::string(buf,ifs_->gcount()));
			ofs <<
				globals_.options_.archiveEncryption_->Encrypt(
					globals_.options_.archiveCompression_->ForceWrite(
							globals_.options_.encoding_->ForceWrite(
								globals_.options_.fileCompression_->ForceWrite(
									std::string(buf,ifs_->gcount()))))
				);
		}

		std::string tmpPreCompression=globals_.options_.fileCompression_->ForceWriteAndClose("");
		std::string tmpEncoding=globals_.options_.encoding_->ForceWriteAndClose(tmpPreCompression);

		ofs <<
				globals_.options_.archiveEncryption_->Encrypt(
					globals_.options_.archiveCompression_->ForceWrite(tmpEncoding+memberTrailer_)
				);
}

size_t XmltarMemberCreate::MaximumSize(size_t n){
	return
				memberHeader_.size()
				+globals_.options_.encoding_->MaximumCompressedtextSizeGivenPlaintextSize(
						globals_.options_.fileCompression_->MaximumCompressedtextSizeGivenPlaintextSize(n)
					)
				+memberTrailer_.size()
			;

}

size_t XmltarMemberCreate::MemberSize(){
	return MaximumSize(file_size_);
}

std::string XmltarMemberCreate::MemberHeader(){
    std::string s;

    s=s+globals_.options_.Tabs("\t\t")+"<file name=\"" + EncodeStringToXMLSafeString(filepath_.relative_path().string()) + "\">"+globals_.options_.Newline();

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

	if (!metadataWritten_){
		s=s+globals_.options_.Tabs("\t\t\t")+"<meta-data>"+globals_.options_.Newline();

		for(unsigned int i=0; i<attr_list.size(); ++i)
			s=s+globals_.options_.Tabs("\t\t\t\t")+"<extended-attribute key=\""
				+EncodeStringToXMLSafeString(attr_list[i].first)+"\" value=\""
				+EncodeStringToXMLSafeString(attr_list[i].second)+"\"/>"+globals_.options_.Newline();

		struct passwd *pw=getpwuid(stat_buf_.st_uid);
		struct group *g=getgrgid(stat_buf_.st_gid);

		s=s+globals_.options_.Tabs("\t\t\t\t")+"<mode value=\""+ToOctalInt(stat_buf_.st_mode)+"\"/>"+globals_.options_.Newline();
		s=s+globals_.options_.Tabs("\t\t\t\t")+"<atime posix=\"" + std::to_string(stat_buf_.st_atime) + "\"/>"+globals_.options_.Newline();
		s=s+globals_.options_.Tabs("\t\t\t\t")+"<ctime posix=\"" + std::to_string(stat_buf_.st_ctime) + "\"/>"+globals_.options_.Newline();
		s=s+globals_.options_.Tabs("\t\t\t\t")+"<mtime posix=\"" + std::to_string(stat_buf_.st_mtime) + "\"/>"+globals_.options_.Newline();

		s=s+globals_.options_.Tabs("\t\t\t\t")+"<user uid=\""+ToDecimalInt(stat_buf_.st_uid)+"\" uname=\""+ (pw!=NULL?pw->pw_name:"") + "\"/>"+globals_.options_.Newline();
		s=s+globals_.options_.Tabs("\t\t\t\t")+"<group gid=\""+ToDecimalInt(stat_buf_.st_gid)+"\" gname=\""+ (g!=NULL?g->gr_name:"") + "\"/>"+globals_.options_.Newline();
		if (S_ISCHR(stat_buf_.st_mode) || S_ISBLK(stat_buf_.st_mode)){
			s=s+globals_.options_.Tabs("\t\t\t\t")+"<rdev value=\""+ToOctalInt(stat_buf_.st_rdev)+"\"/>"+globals_.options_.Newline();
		}
		s=s+globals_.options_.Tabs("\t\t\t\t")+"<size value=\""+ToDecimalInt(stat_buf_.st_size)+"\"/>"+globals_.options_.Newline();

		s=s+globals_.options_.Tabs("\t\t\t")+"</meta-data>"+globals_.options_.Newline();
	}

    switch(f_type_){
        case std::filesystem::file_type::regular:
            s=s+globals_.options_.Tabs("\t\t\t")+"<content type=\"regular\">"+globals_.options_.Newline();
            s=s+globals_.options_.Tabs("\t\t\t\t")+"<stream name=\"data\" pre-compression=\""+globals_.options_.fileCompression_.get()->CompressorName();

            s+=std::string("\" encoding=\"") + globals_.options_.encoding_.get()->CompressorName();

            s+="\" total-size=\""+std::to_string(file_size_)+"\" this-extent-start=\""+std::to_string(ifs_->tellg())+"\">"+globals_.options_.Newline();
            break;
        case std::filesystem::file_type::directory:
            s=s+globals_.options_.Tabs("\t\t\t")+"<content type=\"directory\"/>"+globals_.options_.Newline();
            break;
        case std::filesystem::file_type::symlink:
			{
				std::unique_ptr<char[]> p(new char[stat_buf_.st_size]);
				if (readlink(filepath_.string().c_str(),p.get(),stat_buf_.st_size)!=stat_buf_.st_size)
					throw "Archive_Member::Generate_Metadata: symbolic link size changed";
				s=s+globals_.options_.Tabs("\t\t\t")+"<content type=\"symlink\" target=\""+EncodeStringToXMLSafeString(std::string(p.get(),stat_buf_.st_size))+"\"/>"+globals_.options_.Newline();
			}
            break;
        case std::filesystem::file_type::block:
            s=s+globals_.options_.Tabs("\t\t\t")+"<content type=\"block\"/>"+globals_.options_.Newline();
            break;
        case std::filesystem::file_type::character:
            s=s+globals_.options_.Tabs("\t\t\t")+"<content type=\"character\"/>"+globals_.options_.Newline();
            break;
        case std::filesystem::file_type::fifo:
            s=s+globals_.options_.Tabs("\t\t\t")+"<content type=\"fifo\"/>"+globals_.options_.Newline();
            break;
        case std::filesystem::file_type::socket:
            s=s+globals_.options_.Tabs("\t\t\t")+"<content type=\"socket\"/>"+globals_.options_.Newline();
            break;
        case std::filesystem::file_type::none:
        case std::filesystem::file_type::unknown:
        case std::filesystem::file_type::not_found:
        default:
            throw std::runtime_error("Archive_Member::Generate_Header: unable to stat file");
            break;
    }

    return s;
}

std::string XmltarMemberCreate::MemberTrailer(){
    std::string s;

    // only include a content section if the file is a regular file
    if (f_type_==std::filesystem::file_type::regular){
        //s=s+globals_.options_.Newline();
        s=s+globals_.options_.Tabs("\t\t\t\t")+"</stream>"+globals_.options_.Newline();
    }
    if (f_type_==std::filesystem::file_type::regular){
        s=s+globals_.options_.Tabs("\t\t\t")+"</content>"+globals_.options_.Newline();
    }
    s=s+globals_.options_.Tabs("\t\t")+"</file>"+globals_.options_.Newline();

    return s;
}

std::string XmltarMemberCreate::CompressedMemberHeader(){
	return MemberHeader();
}

std::string XmltarMemberCreate::CompressedMemberTrailer(){
	return MemberTrailer();
}

size_t XmltarMemberCreate::MinimumSize(){
	return
				memberHeader_.size()
				+globals_.options_.encoding_->MaximumCompressedtextSizeGivenPlaintextSize(
						globals_.options_.fileCompression_.get()->MaximumCompressedtextSizeGivenPlaintextSize(1)
					)
				+memberTrailer_.size()
			;
}

size_t XmltarMemberCreate::NumberOfFileBytesThatCanBeArchived(size_t committedBytes, size_t pendingBytes){
	// betz::Debug2 dbg("XmltarMember::NumberOfFileBytesThatCanBeArchived");

	if (globals_.options_.preencryptedTapeLength_.value()<committedBytes+pendingBytes)
		// throw std::logic_error("XmltarMember::NumberOfFileBytesThatCanBeArchived: overflow");
		return 0;

	size_t archiveBytes=globals_.options_.preencryptedTapeLength_.value()-committedBytes-pendingBytes;
	size_t uncompressedArchiveBytes=globals_.options_.archiveCompression_->MinimumPlaintextSizeGivenCompressedtextSize(archiveBytes);
	size_t encodedMemberBytes;
	if ((memberHeader_.size()+memberTrailer_.size())>uncompressedArchiveBytes)
		encodedMemberBytes=0;
	else
		encodedMemberBytes=globals_.options_.encoding_->MinimumPlaintextSizeGivenCompressedtextSize(uncompressedArchiveBytes-memberHeader_.size()-memberTrailer_.size());
	size_t precompressedBytes=globals_.options_.fileCompression_->MinimumPlaintextSizeGivenCompressedtextSize(encodedMemberBytes);

	return precompressedBytes;
#if 0
	size_t numberOfFileBytesThatCanBeArchived
		=	precompression_.value()->MinimumPlaintextSizeGivenCompressedtextSize(
				encoding_.value()->MinimumPlaintextSizeGivenCompressedtextSize(
					memberCompression_.value()->MinimumPlaintextSizeGivenCompressedtextSize(
						archiveCompression.value()->MinimumPlaintextSizeGivenCompressedtextSize(
							options_.preencryptedTapeLength_.value()-committedBytes-pendingBytes-(includeMemberHeader?memberHeader_.size():0)-memberTrailer_.size()))));

	return numberOfFileBytesThatCanBeArchived;
#endif
}

bool XmltarMemberCreate::CanArchiveDirectory(size_t committedBytes, size_t pendingBytes){
	if (globals_.options_.preencryptedTapeLength_.value()<committedBytes+pendingBytes+memberHeader_.size()+memberTrailer_.size()) return false;

	size_t numberOfFileBytesThatCanBeArchived
		=
			globals_.options_.archiveCompression_->MinimumPlaintextSizeGivenCompressedtextSize(
					globals_.options_.preencryptedTapeLength_.value()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size());

	return numberOfFileBytesThatCanBeArchived;
}

bool XmltarMemberCreate::CanArchiveSymLink(size_t committedBytes, size_t pendingBytes){
	if (globals_.options_.preencryptedTapeLength_.value()<committedBytes+pendingBytes+memberHeader_.size()+memberTrailer_.size()) return false;

	size_t numberOfFileBytesThatCanBeArchived
		=
				globals_.options_.archiveCompression_->MinimumPlaintextSizeGivenCompressedtextSize(
						globals_.options_.preencryptedTapeLength_.value()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size());

	return numberOfFileBytesThatCanBeArchived;
}
