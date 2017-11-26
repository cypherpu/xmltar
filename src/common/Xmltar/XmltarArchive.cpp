/*
 * XmltarArchive.cpp
 *
 *  Created on: Nov 21, 2017
 *      Author: dbetz
 */

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
	std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> & filesToBeArchived)
	: options_(opts), filename_(filename), filesToBeArchived_(filesToBeArchived) {}

PartialFileRead XmltarArchive::create(unsigned int volumeNumber){
	std::ofstream ofs(filename_);

	if (options_.multi_volume_ && options_.multi_volume_.get()){
		if (!options_.tape_length_)
			throw std::logic_error("XmltarArchive::create: must specify tape_length for multi-volume archive");

		std::ofstream ofs(filename_);
		std::string compressedHeader=Compress(Header(filename_,volumeNumber));
		std::string minCompressedTrailer=Compress(Trailer(0));

		XmltarMember member(options_,filesToBeArchived_.top(),true);

		std::string memberHeader=member.Header();
		std::string memberTrailer=member.Trailer();

		if (compressedHeader.size()+Compress(memberHeader+memberTrailer).size()+minCompressedTrailer.size())
				;

		// write gzip Archive header

		// while(maxLength(archiveHeader+memberHeader)<tapelength){
		//		archive at least part of next volume
	}
}

PartialFileRead XmltarArchive::append(unsigned int volumeNumber)
{
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

	std::string uncompressedContent=Decompress(compressedContent);

	return IsPaddingTrailer(uncompressedContent);
}

std::string XmltarArchive::Compress(std::string s){
	std::string result;
	Bidirectional_Pipe p;

	p.Open(
			CompressionCommand(options_.archiveMemberCompression_.get()),
			CompressionArguments(options_.archiveMemberCompression_.get()));

	if (!p.ChildExitedAndAllPipesClosed() && p.Can_Write()){
		p.QueueWrite(s);
		p.QueueWriteClose();
	}
	while(!p.ChildExitedAndAllPipesClosed()){
		if (p.Can_Read1()) result+=p.Read1();
		if (p.Can_Read2()) p.Read2();
	}

	return result;
}

std::string XmltarArchive::Decompress(std::string s){
	std::string result;
	Bidirectional_Pipe p;

	p.Open(
			CompressionCommand(options_.archiveMemberCompression_.get()),
			DecompressionArguments(options_.archiveMemberCompression_.get()));

	if (!p.ChildExitedAndAllPipesClosed() && p.Can_Write()){
		p.QueueWrite(s);
		p.QueueWriteClose();
	}
	while(!p.ChildExitedAndAllPipesClosed()){
		if (p.Can_Read1()) result+=p.Read1();
		if (p.Can_Read2()) p.Read2();
	}

	return result;
}

std::string XmltarArchive::Header(std::string filename, int archive_sequence_number){
    std::string s;

    s+="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"+options_.Newline();
    s+="<xmltar xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">"+options_.Newline();
    s+=options_.Tabs("\t")+"<archive-name>"+filename+"</archive-name>"+options_.Newline();
    s+=options_.Tabs("\t")+"<members>"+options_.Newline();

    return s;
}

std::string XmltarArchive::Trailer(unsigned int padding){
	NonDeterministicRNG nonDetRNG;
	boost::random::uniform_int_distribution<> uniform(0,15);

    std::string s
		=options_.Tabs("\t")+"</members>"+options_.Newline()
		+options_.Tabs("\t")+"<padding>";

    for(int i=0; i<padding; ++i)
    	s+=ToHexDigit(uniform(nonDetRNG));

    s
		+="</padding>"+options_.Newline();
    	 +"</xmltar>"+options_.Newline();

    return s;
}
