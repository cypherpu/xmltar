/*
 * XmltarArchive.cpp
 *
 *  Created on: Nov 21, 2017
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

#include <boost/circular_buffer.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int.hpp>

#include "Xmltar/XmltarArchive.hpp"
#include "Xmltar/XmltarMember.hpp"
#include "Utilities/ToHexDigit.hpp"
#include "Bidirectional_Pipe.hpp"

#include "Transform/TransformIdentity.hpp"

class NonDeterministicRNG : public boost::random::random_device {
public:
	NonDeterministicRNG()
		: boost::random::random_device("/dev/urandom")				// TODO: change this to /dev/random once we have a better entropy source
	{}
};

class CBEntry {
public:
	char c_;
	std::ios::off_type offset_;

	CBEntry(char c, std::ios::off_type offset)
		: c_(c), offset_(offset){}

	CBEntry(std::fstream & iofs){
		std::ios::off_type offset__=iofs.tellg();
		iofs.get(c_);
	}
};

XmltarArchive::XmltarArchive(
	XmltarOptions & opts,
	std::string filename,
	unsigned int volumeNumber,
	std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> & filesToBeArchived,
	std::shared_ptr<XmltarMember> nextMember
)
	: options_(opts), volumeNumber_(volumeNumber), filename_(filename), filesToBeArchived_(filesToBeArchived), nextMember_(nextMember)
{
	std::shared_ptr<Transform> archiveCompression(options_.archiveCompression_.get()->clone());

	if (options_.operation_.get()==XmltarOptions::Operation::CREATE)
		if (options_.multi_volume_){
			if (!options_.tape_length_)
				throw std::runtime_error("XmltarArchive::XmltarArchive: --tape-length must be specified when creating multi-volume archive");

			std::ofstream ofs(filename_);
			std::string compressedArchiveHeader=CompressedArchiveHeader(filename_,volumeNumber);
			std::string compressedArchiveTrailer=CompressedArchiveTrailer(0);

			if (options_.tape_length_.get()<compressedArchiveHeader.size()+compressedArchiveTrailer.size())
				throw std::logic_error("XmltarArchive::XmltarArchive: archive too small to hold header and trailer of archive member");

			ofs << compressedArchiveHeader;
			ofs.flush(); // DEBUG
			size_t committedBytes=compressedArchiveHeader.size();
			size_t pendingBytes=compressedArchiveTrailer.size();

			std::cerr << "compressedArchiveHeader.size()= " << compressedArchiveHeader.size() << std::endl;
			std::cerr << "compressedArchiveTrailer.size()=" << compressedArchiveTrailer.size() << std::endl;

			archiveCompression.get()->OpenCompression();

			if (!filesToBeArchived.empty() && !nextMember_)
				nextMember_=NextMember();

			bool includeMemberHeader=true;
			for(bool firstPass=true; nextMember_; firstPass=false){
				std::cerr << "committedBytes=" << committedBytes << std::endl;
				std::cerr << "pendingBytes=  " << pendingBytes << std::endl;
				std::cerr << "file=" << nextMember_->filepath() << std::endl;

				if (nextMember_->isDirectory()){
					if (nextMember_->CanArchiveDirectory(committedBytes, pendingBytes, archiveCompression)){
						std::string compressedDirectoryMember
							= options_.archiveMemberCompression_->CompressString(
									nextMember_->MemberHeader()+nextMember_->MemberTrailer()
								);
						archiveCompression->Write(compressedDirectoryMember);
						nextMember_=NextMember();
						includeMemberHeader=true;
						pendingBytes+=compressedDirectoryMember.size();
					}
					else if (firstPass)
						throw std::logic_error("XmltarArchive::XmltarArchive: archive too small to hold directory archive member");
					else {
						ofs << archiveCompression->Close();
						ofs.flush();
					}
				}
				else {
					size_t numberOfFileBytesThatCanBeArchived=nextMember_->NumberOfFileBytesThatCanBeArchived(committedBytes,pendingBytes,archiveCompression,includeMemberHeader);
					std::cerr << "XmltarArchive: archiving " << numberOfFileBytesThatCanBeArchived << " of " << nextMember_->filepath().string() << std::endl;
					if (numberOfFileBytesThatCanBeArchived==0)
						if (firstPass)
							throw std::logic_error("XmltarArchive::XmltarArchive: archive too small to hold even 1 char of archive member");
						else {	// close off this archiveCompression to free up space
							ofs << archiveCompression.get()->Close();
							committedBytes=archiveCompression.get()->ReadCount();
							pendingBytes=compressedArchiveTrailer.size();
							numberOfFileBytesThatCanBeArchived=nextMember_->NumberOfFileBytesThatCanBeArchived(committedBytes,pendingBytes,archiveCompression,includeMemberHeader);
							if (numberOfFileBytesThatCanBeArchived==0){
								if (committedBytes+compressedArchiveTrailer.size()<=options_.tape_length_.get()){
									while(committedBytes+compressedArchiveTrailer.size()+
											archiveCompression->EmptyCompressedSize()>options_.tape_length_.get()){
										std::string tmp=archiveCompression->CompressString("");
										ofs << tmp;
										committedBytes+=tmp.size();
									}

								}
								else
									throw std::logic_error("XmltarARchive::XmltarArchive: overflow");
							}
							else
								archiveCompression.reset(archiveCompression.get()->clone());
						}

					nextMember_->write(archiveCompression,numberOfFileBytesThatCanBeArchived,ofs);
					pendingBytes=archiveCompression->MaximumCompressedtextSizeGivenPlaintextSize(archiveCompression->WriteCount());
					if (nextMember_->IsComplete()){
						nextMember_=NextMember();
						includeMemberHeader=true;
					}
					else
						includeMemberHeader=true;
				}

				if (nextMember_->filepath().string()=="src/common"){
					ofs << archiveCompression->Close(); ofs.flush(); exit(0); // DEBUG
				}

			}
		}
		else {
			std::ofstream ofs(filename_);
			std::string compressedHeader=CompressedArchiveHeader(filename_,volumeNumber);
			std::string minCompressedTrailer=CompressedArchiveTrailer(0);

			std::cerr << "XmltarArchive::XmltarArchive: " << filesToBeArchived.size() << std::endl;
			std::shared_ptr<XmltarMember> xmltarMember;
			for( ; filesToBeArchived.size(); ){
				std::cerr << "XmltarArchive::XmltarArchive: " << filesToBeArchived.top() << std::endl;

				boost::filesystem::path const filepath=filesToBeArchived_.top();
				filesToBeArchived.pop();
				boost::filesystem::file_status f_stat=boost::filesystem::symlink_status(filepath);

				if (boost::filesystem::is_directory(f_stat)){
					for(auto & p : boost::filesystem::directory_iterator(filepath) ){
						filesToBeArchived.push(p);
					}
				}

				xmltarMember=std::make_shared<XmltarMember>(options_,filepath);
			}
		}
}

PartialFileRead XmltarArchive::append(unsigned int volumeNumber)
{
#if 0
	// we have to back-track to find the end of the last member in the archive
	boost::format fmt(options_.base_xmltar_file_name_.get());
	fmt % volumeNumber;
	std::string filename=str(fmt);

	if (!boost::filesystem::exists(filename)){
		return create(volumeNumber);
	}
	else {
		std::fstream iofs(filename);
		std::string magic=HeaderMagicNumber(options_.archiveMemberCompression_.get(),"<padding>");

		// create a list of possible trailer starts
		std::vector<std::ios::pos_type> positions;
		boost::circular_buffer<CBEntry> cb(magic.size());
		for(;;){
			while(iofs)
				if (cb.size()<magic.size())
					cb.push_back(CBEntry(iofs));
			if (!iofs && cb.size()<magic.size()) break;

			int matchLen;
			for( matchLen=0; matchLen<magic.size(); ++matchLen)
				if (cb[matchLen].c_!=magic[matchLen]) break;

			if (matchLen==magic.size())
				positions.push_back(cb[0].offset_);

			cb.pop_front();
		}

		for( ; ; ){
			// find likely gzipped trailer
			for( ; positions.size()>0; positions.pop_back()){
				iofs.seekg(positions.back());

				std::string content((std::istreambuf_iterator<char>(iofs) ),
									(std::istreambuf_iterator<char>()    ) );

				if (IsCompressedPaddingTrailer(iofs, positions.back()))
					break;
			}

			// couldn't find gzipped trailer
			if (positions.size()==0)
				throw std::logic_error("XmltarArchive::XmltarArchive: unable to identify trailer in xmltar file");

			// try to decompress up to this gzipped trailer
			iofs.seekg(0,std::ios_base::beg);

			Bidirectional_Pipe p;
			p.Open(
				CompressionCommand(options_.archiveMemberCompression_.get()),
				DecompressionArguments(options_.archiveMemberCompression_.get())
			);

			for(char c; iofs; iofs.get(c))
				if (iofs.tellg()==positions.back()) break;
				else {
					if (!p.ChildExitedAndAllPipesClosed() && p.Can_Write()){
						p.QueueWrite(c);
					}
					while(!p.ChildExitedAndAllPipesClosed()){
						if (p.Can_Read1()) p.Read1();
						if (p.Can_Read2()) p.Read2();
					}
				}

			p.QueueWriteClose();
			while(!p.ChildExitedAndAllPipesClosed()){
				if (p.Can_Read1()) p.Read1();
				if (p.Can_Read2()) p.Read2();
			}

			if (p.ExitStatus()==0) break;
		}

		iofs.seekg(positions.back());

		// we have found the trailer and the stream is positioned to overwrite the old trailer
	}
#endif
}

bool XmltarArchive::IsPaddingTrailer(std::string s){
	std::string openingPaddingTag="<padding>";
	std::string closingPaddingTag="</padding>";
	std::string closingArchiveTag="</xmltar>";

	std::string::size_type i=0;
	for( ; i<s.size(); ++i)
		if (!std::isspace(s[i])) break;

	if (i+openingPaddingTag.size()<=s.size() && s.substr(i,openingPaddingTag.size())==openingPaddingTag)
		i+=openingPaddingTag.size();
	else return false;

	for( ; i<s.size(); ++i)
		if (!std::isxdigit(s[i])) break;

	if (i+closingPaddingTag.size()<=s.size() && s.substr(i,closingPaddingTag.size())==closingPaddingTag)
		i+=closingPaddingTag.size();
	else return false;

	for( ; i<s.size(); ++i)
		if (!std::isspace(s[i])) break;

	if (i+closingArchiveTag.size()<=s.size() && s.substr(i,closingArchiveTag.size())==closingArchiveTag)
		i+=closingArchiveTag.size();
	else return false;

	if (i==s.size()) return true;
	else if (i==s.size()-1 && s[i]=='\n') return true;

	return false;
}

bool XmltarArchive::IsCompressedPaddingTrailer(std::fstream & iofs, std::ios::off_type offset){
	// We try to uncompress from file::offset to the end.
	// If it succeeds, and we match string s to the beginning, we are likely in the correct spot.

	iofs.seekg(offset);
	std::string compressedContent(	(std::istreambuf_iterator<char>(iofs)),
									(std::istreambuf_iterator<char>()    ));

	std::string uncompressedContent=options_.archiveCompression_.get()->DecompressString(
										options_.archiveMemberCompression_.get()->DecompressString(
											compressedContent
										)
									);

	return IsPaddingTrailer(uncompressedContent);
}

std::string XmltarArchive::ArchiveHeader(std::string filename, int archive_sequence_number){
    std::string s;

    s+="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"+options_.Newline();
    s+="<xmltar xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">"+options_.Newline();
    s+=options_.Tabs("\t")+"<archive-name>"+filename+"</archive-name>"+options_.Newline();
    s+=options_.Tabs("\t")+"<members>"+options_.Newline();

    return s;
}

std::string XmltarArchive::CompressedArchiveHeader(std::string filename, int archive_sequence_number){
	return options_.archiveCompression_.get()->CompressString(
				options_.archiveMemberCompression_.get()->CompressString(
						ArchiveHeader(filename, archive_sequence_number)
				)
			);
}

std::string XmltarArchive::ArchiveTrailerBegin(){
	NonDeterministicRNG nonDetRNG;
	boost::random::uniform_int_distribution<> uniform(0,15);

    std::string s
		=options_.Tabs("\t")+"</members>"+options_.Newline()
		+options_.Tabs("\t")+"<padding>";

    return s;
}

std::string XmltarArchive::ArchiveTrailerMiddle(unsigned int padding){
	std::string s=options_.archiveCompression_.get()->MinimumCompressionString();

    return s;
}

std::string XmltarArchive::ArchiveTrailerEnd(){
    std::string s="</padding>"+options_.Newline()+"</xmltar>"+options_.Newline();

    return s;
}

std::string XmltarArchive::ArchiveTrailer(unsigned int padding){
	return ArchiveTrailerBegin()+ArchiveTrailerMiddle(padding)+ArchiveTrailerEnd();
}

std::string XmltarArchive::CompressedArchiveTrailer(unsigned int padding){
	return options_.archiveCompression_.get()->CompressString(
				options_.archiveMemberCompression_.get()->CompressString(
								ArchiveTrailerBegin()
				)
			)
			+options_.archiveCompression_.get()->CompressString(
				options_.archiveMemberCompression_.get()->CompressString(
								ArchiveTrailerMiddle(padding)
				)
			)
			+options_.archiveCompression_.get()->CompressString(
				options_.archiveMemberCompression_.get()->CompressString(
								ArchiveTrailerEnd()
				)
			);
}
