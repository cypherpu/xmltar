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

    if (lstat(filepath_.string().c_str(),&stat_buf)!=0)
        throw "Archive_Member:Archive_Member: cannot lstat file";

    memberHeader_=MemberHeader();
    memberTrailer_=MemberTrailer();
}

void XmltarMember::write(std::shared_ptr<Transform> archiveCompression, size_t committedBytes, size_t pendingBytes, std::ostream & ofs){
		size_t numberOfFileBytesThatCanBeArchived=NumberOfFileBytesThatCanBeArchived(committedBytes,pendingBytes,archiveCompression);

		betz::Debug2 dbg("XmltarMember::write");
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
}

std::string XmltarMember::MemberHeader(){
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

    switch(f_type){
        case boost::filesystem::regular_file:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"regular\">"+options_.Newline();
            s=s+options_.Tabs("\t\t\t\t")+"<stream name=\"data\" pre-compression=\""+options_.fileCompression_.get()->CompressionName();

            s+=std::string("\" encoding=\"") + options_.encoding_.get()->CompressionName();

            s+="\" total-size=\""+std::to_string(file_size)+"\" this-extent-start=\""+std::to_string(nextByte_)+"\">"+options_.Newline();
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
            s+=options_.Tabs("\t\t\t\t")+"<symlink target=\""+XmlEscapeAttribute(CppStringEscape(std::string(p.get(),stat_buf.st_size)))+"\"/>"+options_.Newline();
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

std::string XmltarMember::CompressedMemberHeader(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberHeader());
}

std::string XmltarMember::CompressedMemberTrailer(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberTrailer());
}

size_t XmltarMember::NumberOfFileBytesThatCanBeArchived(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
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

bool XmltarMember::IsComplete(){
	return nextByte_==file_size;
}

boost::filesystem::path XmltarMember::filepath(){
	return filepath_;
}

bool XmltarMember::isDirectory(){
	return f_type==boost::filesystem::file_type::directory_file;
}

bool XmltarMember::isSymLink(){
	return f_type==boost::filesystem::file_type::symlink_file;
}

bool XmltarMember::isRegularFile(){
	return f_type==boost::filesystem::file_type::regular_file;
}

size_t XmltarMember::NextByte(){
	return nextByte_;
}

void XmltarMember::RecalculateMemberHeader(){
	memberHeader_=MemberHeader();
}

