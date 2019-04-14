/*
 * XmltarMember.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dbetz
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

#include "Xmltar/XmltarMemberCreate.hpp"
#include "Utilities/XMLSafeString.hpp"
#include "Utilities/ToLocalTime.hpp"
#include "Utilities/ToDecimalInt.hpp"
#include "Utilities/ToOctalInt.hpp"
#include "Utilities/PrintOpenFileDescriptors.hpp"
#include "Transform/TransformHex.hpp"
#include "../Debug2/Debug2.hpp"

XmltarMemberCreate::XmltarMemberCreate(XmltarOptions const & options, std::filesystem::path const & filepath)
	: options_(options), filepath_(filepath), metadataWritten_(false) {
	// betz::Debug dbg("XmltarMember::XmltarMember");

    f_stat=std::filesystem::symlink_status(filepath_);

    if (!std::filesystem::exists(f_stat))
        throw "Archive_Member::Archive_Member: source file does not exist: "+filepath_.string();

    f_type=f_stat.type();

    if (std::filesystem::is_regular_file(f_stat)){
    	std::cerr << "########### is regular file" << std::endl;
        file_size=std::filesystem::file_size(filepath_);
        ifs_.reset(new std::ifstream(filepath_.string()));
    	std::cerr << "########### running sha3sum512_" << std::endl;
        sha3sum512_.run();
    }
    else file_size=0;

    if (lstat(filepath_.string().c_str(),&stat_buf)!=0)
        throw "Archive_Member:Archive_Member: cannot lstat file";

    memberHeader_=MemberHeader();
    memberTrailer_=MemberTrailer();

    startingVolume_=options_.current_volume_;
}

XmltarMemberCreate::~XmltarMemberCreate(){
	std::string output;

	if (std::filesystem::is_regular_file(f_stat)){
		output=sha3sum512_.ForceWriteAndClose("");
    }

	if (options_.listed_incremental_file_){
		*options_.incrementalFileOfs_.get()
			<< "\t<file name=\"" << EncodeStringToXMLSafeString(filepath_.string()) << "\">\n"
			<< "\t\t" << SnapshotEvent(options_.invocationTime_,startingVolume_,output,options_.dump_level_.get()) << std::endl
			<< "\t</file>" << std::endl;
		std::cerr << "Digest=" << output << std::endl;
	}
}

void XmltarMemberCreate::write(std::shared_ptr<Transform> archiveCompression, size_t numberOfFileBytesThatCanBeArchived, std::ostream & ofs){
		betz::Debug2 dbg("XmltarMember::write");
		std::cerr << dbg << ": numberOfFileBytesThatCanBeArchived=" << numberOfFileBytesThatCanBeArchived << std::endl;
		// std::ifstream ifs(filepath_.string());
		// ifs.seekg(nextByte_);

		std::shared_ptr<Transform> precompression(options_.fileCompression_->clone());
		std::shared_ptr<Transform> memberCompression(options_.archiveMemberCompression_->clone());
		std::shared_ptr<Transform> encoding(options_.encoding_->clone());

		size_t numberOfBytesToArchive=std::min(file_size-ifs_->tellg(),(off_t)numberOfFileBytesThatCanBeArchived);
		precompression->OpenCompression();
		encoding->OpenCompression();
		memberCompression->OpenCompression();
		char buf[1024];

		std::cerr << dbg << ": memberHeader_=" << memberHeader_.size() << std::endl;
		ofs << archiveCompression->ForceWrite(memberCompression->ForceWrite(memberHeader_));
		metadataWritten_=true;
	    memberHeader_=MemberHeader();
		std::cerr << dbg << ": after memberCompression-ForceWrite" << std::endl;

		for( size_t i=numberOfBytesToArchive; *ifs_ && i>0; i-=ifs_->gcount()){
			ifs_->read(buf,std::min((size_t)i,sizeof(buf)));
			sha3sum512_.ForceWrite(std::string(buf,ifs_->gcount()));
			ofs <<
				archiveCompression->ForceWrite(
					memberCompression->ForceWrite(
						encoding->ForceWrite(
							precompression->ForceWrite(
								std::string(buf,ifs_->gcount())))));
		}

		std::cerr << dbg << ": after read" << std::endl;

		std::string tmpPreCompression=precompression->ForceWriteAndClose("");
		std::cerr << dbg << ": after tmpPreCompression" << std::endl;

		std::cerr << "tmpPreCompression.size()=" << tmpPreCompression.size() << std::endl;
		std::string tmpEncoding=encoding->ForceWriteAndClose(tmpPreCompression);
		std::cerr << dbg << ": after tmpEncoding" << std::endl;
		std::string tmpMemberCompression=memberCompression->ForceWriteAndClose(tmpEncoding+memberTrailer_);

		ofs <<
			archiveCompression->ForceWrite(tmpMemberCompression);

#if 0
		ofs <<
			archiveCompression->ForceWrite(
				memberCompression->ForceWriteAndClose(
					encoding->ForceWriteAndClose(
						precompression->ForceWriteAndClose(""))));

#endif
		std::cerr << dbg << ": precompression->ReadCount=" << precompression->ReadCount() << std::endl;
		std::cerr << dbg << ": precompression->WriteCount=" << precompression->WriteCount() << std::endl;
		std::cerr << dbg << ": encoding->ReadCount=" << encoding->ReadCount() << std::endl;
		std::cerr << dbg << ": encoding->WriteCount=" << encoding->WriteCount() << std::endl;
		std::cerr << dbg << ": memberCompression->ReadCount=" << memberCompression->ReadCount() << std::endl;
		std::cerr << dbg << ": memberCompression->WriteCount=" << memberCompression->WriteCount() << std::endl;
}

size_t XmltarMemberCreate::MaximumSize(size_t n){
	return
			options_.archiveMemberCompression_->MaximumCompressedtextSizeGivenPlaintextSize(
				memberHeader_.size()
				+options_.encoding_->MaximumCompressedtextSizeGivenPlaintextSize(
						options_.fileCompression_->MaximumCompressedtextSizeGivenPlaintextSize(n)
					)
				+memberTrailer_.size()
			);

}

size_t XmltarMemberCreate::MemberSize(){
	return MaximumSize(file_size);
}

std::string XmltarMemberCreate::MemberHeader(){
    std::string s;

    s=s+options_.Tabs("\t\t")+"<file name=\"" + EncodeStringToXMLSafeString(filepath_.relative_path().string()) + "\">"+options_.Newline();

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
		s=s+options_.Tabs("\t\t\t")+"<meta-data>"+options_.Newline();

		for(unsigned int i=0; i<attr_list.size(); ++i)
			s=s+options_.Tabs("\t\t\t\t")+"<extended-attribute key=\""
				+EncodeStringToXMLSafeString(attr_list[i].first)+"\" value=\""
				+EncodeStringToXMLSafeString(attr_list[i].second)+"\"/>"+options_.Newline();

		struct passwd *pw=getpwuid(stat_buf.st_uid);
		struct group *g=getgrgid(stat_buf.st_gid);

		s=s+options_.Tabs("\t\t\t\t")+"<mode value=\""+ToOctalInt(stat_buf.st_mode)+"\"/>"+options_.Newline();
		s=s+options_.Tabs("\t\t\t\t")+"<atime posix=\"" + std::to_string(stat_buf.st_atime) + "\"/>"+options_.Newline();
		s=s+options_.Tabs("\t\t\t\t")+"<ctime posix=\"" + std::to_string(stat_buf.st_ctime) + "\"/>"+options_.Newline();
		s=s+options_.Tabs("\t\t\t\t")+"<mtime posix=\"" + std::to_string(stat_buf.st_mtime) + "\"/>"+options_.Newline();

		s=s+options_.Tabs("\t\t\t\t")+"<user uid=\""+ToDecimalInt(stat_buf.st_uid)+"\" uname=\""+ (pw!=NULL?pw->pw_name:"") + "\"/>"+options_.Newline();
		s=s+options_.Tabs("\t\t\t\t")+"<group gid=\""+ToDecimalInt(stat_buf.st_gid)+"\" gname=\""+ (g!=NULL?g->gr_name:"") + "\"/>"+options_.Newline();
		if (S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode)){
			s=s+options_.Tabs("\t\t\t\t")+"<rdev value=\""+ToOctalInt(stat_buf.st_rdev)+"\"/>"+options_.Newline();
		}
		s=s+options_.Tabs("\t\t\t\t")+"<size value=\""+ToDecimalInt(stat_buf.st_size)+"\"/>"+options_.Newline();

		s=s+options_.Tabs("\t\t\t")+"</meta-data>"+options_.Newline();
	}

    switch(f_type){
        case std::filesystem::file_type::regular:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"regular\">"+options_.Newline();
            s=s+options_.Tabs("\t\t\t\t")+"<stream name=\"data\" pre-compression=\""+options_.fileCompression_.get()->CompressionName();

            s+=std::string("\" encoding=\"") + options_.encoding_.get()->CompressionName();

            s+="\" total-size=\""+std::to_string(file_size)+"\" this-extent-start=\""+std::to_string(ifs_->tellg())+"\">"+options_.Newline();
            break;
        case std::filesystem::file_type::directory:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"directory\"/>"+options_.Newline();
            break;
        case std::filesystem::file_type::symlink:
			{
				std::unique_ptr<char[]> p(new char[stat_buf.st_size]);
				if (readlink(filepath_.string().c_str(),p.get(),stat_buf.st_size)!=stat_buf.st_size)
					throw "Archive_Member::Generate_Metadata: symbolic link size changed";
				s=s+options_.Tabs("\t\t\t")+"<content type=\"symlink\" target=\""+EncodeStringToXMLSafeString(std::string(p.get(),stat_buf.st_size))+"\"/>"+options_.Newline();
			}
            break;
        case std::filesystem::file_type::block:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"block\"/>"+options_.Newline();
            break;
        case std::filesystem::file_type::character:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"character\"/>"+options_.Newline();
            break;
        case std::filesystem::file_type::fifo:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"fifo\"/>"+options_.Newline();
            break;
        case std::filesystem::file_type::socket:
            s=s+options_.Tabs("\t\t\t")+"<content type=\"socket\"/>"+options_.Newline();
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
    if (f_type==std::filesystem::file_type::regular){
        s=s+options_.Newline();
        s=s+options_.Tabs("\t\t\t\t")+"</stream>"+options_.Newline();
    }
    if (f_type==std::filesystem::file_type::regular){
        s=s+options_.Tabs("\t\t\t")+"</content>"+options_.Newline();
    }
    s=s+options_.Tabs("\t\t")+"</file>"+options_.Newline();

    return s;
}

std::string XmltarMemberCreate::CompressedMemberHeader(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberHeader());
}

std::string XmltarMemberCreate::CompressedMemberTrailer(){
	return options_.archiveMemberCompression_.get()->CompressString(MemberTrailer());
}

size_t XmltarMemberCreate::MinimumSize(){
	return
			options_.archiveMemberCompression_->MaximumCompressedtextSizeGivenPlaintextSize(
				memberHeader_.size()
				+options_.encoding_->MaximumCompressedtextSizeGivenPlaintextSize(
						options_.fileCompression_.get()->MaximumCompressedtextSizeGivenPlaintextSize(1)
					)
				+memberTrailer_.size()
			);
}

size_t XmltarMemberCreate::NumberOfFileBytesThatCanBeArchived(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
	betz::Debug2 dbg("XmltarMember::NumberOfFileBytesThatCanBeArchived");

	if (options_.tape_length_.get()<committedBytes+pendingBytes)
		// throw std::logic_error("XmltarMember::NumberOfFileBytesThatCanBeArchived: overflow");
		return 0;

	size_t archiveBytes=options_.tape_length_.get()-committedBytes-pendingBytes;
	std::cerr << dbg << ": archiveBytes=" << archiveBytes << std::endl;
	size_t uncompressedArchiveBytes=archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(archiveBytes);
	size_t uncompressedMemberBytes=options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(uncompressedArchiveBytes);
	size_t encodedMemberBytes;
	if ((memberHeader_.size()+memberTrailer_.size())>uncompressedMemberBytes)
		encodedMemberBytes=0;
	else
		encodedMemberBytes=options_.encoding_->MinimumPlaintextSizeGivenCompressedtextSize(uncompressedMemberBytes-memberHeader_.size()-memberTrailer_.size());
	size_t precompressedBytes=options_.fileCompression_->MinimumPlaintextSizeGivenCompressedtextSize(encodedMemberBytes);

	std::cerr << dbg << ": archiveBytes=" << archiveBytes << std::endl;
	std::cerr << dbg << ": uncompressedArchiveBytes= " << std::right << std::setw(6) << uncompressedArchiveBytes << std::endl;
	std::cerr << dbg << ": uncompressedMemberBytes=  " << std::right << std::setw(6) << uncompressedMemberBytes << std::endl;
	std::cerr << dbg << ": encodedMemberBytes=       " << std::right << std::setw(6) << encodedMemberBytes << std::endl;
	std::cerr << dbg << ":     memberHeader_.size()=     " << std::right << std::setw(6) << memberHeader_.size() << std::endl;
	std::cerr << dbg << ":     memberTrailer_.size()=    " << std::right << std::setw(6) << memberTrailer_.size() << std::endl;
	std::cerr << dbg << ": precompressedBytes=       " << std::right << std::setw(6) << precompressedBytes << std::endl;

	return precompressedBytes;
#if 0
	size_t numberOfFileBytesThatCanBeArchived
		=	precompression_.get()->MinimumPlaintextSizeGivenCompressedtextSize(
				encoding_.get()->MinimumPlaintextSizeGivenCompressedtextSize(
					memberCompression_.get()->MinimumPlaintextSizeGivenCompressedtextSize(
						archiveCompression.get()->MinimumPlaintextSizeGivenCompressedtextSize(
							options_.tape_length_.get()-committedBytes-pendingBytes-(includeMemberHeader?memberHeader_.size():0)-memberTrailer_.size()))));

	return numberOfFileBytesThatCanBeArchived;
#endif
}

bool XmltarMemberCreate::CanArchiveDirectory(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
	if (options_.tape_length_.get()<committedBytes+pendingBytes+memberHeader_.size()+memberTrailer_.size()) return false;

	std::cerr << "XmltarMember::CanArchiveDirectory:"
			<< " options_.tape_length_.get()=" << options_.tape_length_.get()
			<< " committedBytes=" << committedBytes
			<< " pendingBytes=" << pendingBytes
			<< " memberHeader_.size()=" << memberHeader_.size()
			<< " memberTrailer_.size()=" << memberTrailer_.size()
			<< std::endl;

	size_t numberOfFileBytesThatCanBeArchived
		=	options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(
				archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(
					options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size()));

	std::cerr << "numberOfFileBytesThatCanBeArchived=" << numberOfFileBytesThatCanBeArchived << std::endl;
	std::cerr << "archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size())="
			<< archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size())
			<< std::endl;
	std::cerr << "options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size()))"
			<< options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(
					archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(
						options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size()))
			<< std::endl;

	return numberOfFileBytesThatCanBeArchived;
}

bool XmltarMemberCreate::CanArchiveSymLink(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression){
	if (options_.tape_length_.get()<committedBytes+pendingBytes+memberHeader_.size()+memberTrailer_.size()) return false;

	size_t numberOfFileBytesThatCanBeArchived
		=	options_.archiveMemberCompression_->MinimumPlaintextSizeGivenCompressedtextSize(
				archiveCompression->MinimumPlaintextSizeGivenCompressedtextSize(
					options_.tape_length_.get()-committedBytes-pendingBytes-memberHeader_.size()-memberTrailer_.size()));

	return numberOfFileBytesThatCanBeArchived;
}