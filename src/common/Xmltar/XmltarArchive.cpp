/*

XmltarArchive.cpp
Copyright 2017-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: Nov 21, 2017
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
#include "Xmltar/XmltarMemberCreate.hpp"
#include "Utilities/ToHexDigit.hpp"
#include "Utilities/IsPrefixPath.hpp"
#include "Utilities/Glob.hpp"

#include "XmlParser/XmlParser.hpp"

#include "Generated/Utilities/Debug2.hpp"

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
		offset_=iofs.tellg();
		iofs.get(c_);
	}
};

XmltarArchive::XmltarArchive(
	XmltarGlobals & globals,
	std::string filename,
	unsigned int volumeNumber
)
	: globals_(globals), filename_(filename), volumeNumber_(volumeNumber)
{
}

XmltarArchive::XmltarArchive(XmltarGlobals & globals, std::string filename)
: globals_(globals), filename_(filename), volumeNumber_(0)
{
	if (globals_.options_.operation_.value()==XmltarOptions::Operation::EXTRACT){
		if (globals_.options_.multi_volume_){
			std:: ifstream ifs(filename);

			if (!ifs)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" cannot be read");
#if 0
			char smallBuf[5];
			ifs.read(smallBuf,sizeof(smallBuf));
			if (!ifs)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" error or EOF");
			if (ifs.gcount()!=5)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" too short");
			std::string bufString(smallBuf,sizeof(smallBuf));

			std::vector<std::shared_ptr<Transform>> transformations;
			if (bufString==TransformIdentity::StaticHeaderMagicNumber("<?xml"))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformIdentity>("archiveCompression"));
			else if (bufString.substr(2)==TransformGzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformGzip>("archiveCompression"));
			else if (bufString.substr(3)==TransformBzip2::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformBzip2>("archiveCompression"));
			else if (bufString.substr(5)==TransformLzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformLzip>("archiveCompression"));

			transformations[0]->OpenDecompression();
			std::string readString=transformations[0]->ForceWrite(bufString);
			char buffer[2014];
			while(ifs && readString.size()<5){
				ifs.read(buffer,sizeof(buffer));
				readString=transformations[0]->ForceWrite(std::string(buffer,ifs.gcount()));
			}

			if (readString.size()<5)
				throw std::runtime_error("XmltarArchive::XmltarArchive: readString.size()<5");
			if (readString==TransformIdentity::StaticHeaderMagicNumber("<?xml"))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformIdentity>("memberCompression"));
			else if (readString.substr(2)==TransformGzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformGzip>("memberCompression"));
			else if (readString.substr(3)==TransformBzip2::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformBzip2>("memberCompression"));
			else if (readString.substr(5)==TransformLzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformLzip>("memberCompression"));
#endif
		}
		else {
#if 0
			std:: ifstream ifs(filename);

			if (!ifs)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" cannot be read");

			char smallBuf[5];
			ifs.read(smallBuf,sizeof(smallBuf));
			if (!ifs)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" error or EOF");
			if (ifs.gcount()!=5)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" too short");
			std::string bufString(smallBuf,sizeof(smallBuf));

			std::vector<std::shared_ptr<Transform>> transformations;
			if (bufString==TransformIdentity::StaticHeaderMagicNumber("<?xml"))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformIdentity>("archiveCompression"));
			else if (bufString.substr(2)==TransformGzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformGzip>("archiveCompression"));
			else if (bufString.substr(3)==TransformBzip2::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformBzip2>("archiveCompression"));
			else if (bufString.substr(5)==TransformLzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformLzip>("archiveCompression"));

			transformations[0]->OpenDecompression();
			std::string readString=transformations[0]->ForceWrite(bufString);
			char buffer[2014];
			while(ifs && readString.size()<5){
				ifs.read(buffer,sizeof(buffer));
				readString+=transformations[0]->ForceWrite(std::string(buffer,ifs.gcount()));
			}

			if (readString.size()<5)
				throw std::runtime_error("XmltarArchive::XmltarArchive: readString.size()<5");
			if (readString==TransformIdentity::StaticHeaderMagicNumber("<?xml"))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformIdentity>("memberCompression"));
			else if (readString.substr(2)==TransformGzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformGzip>("memberCompression"));
			else if (readString.substr(3)==TransformBzip2::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformBzip2>("memberCompression"));
			else if (readString.substr(5)==TransformLzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformLzip>("memberCompression"));

			XmltarMultiVolumeXmlHandler archiveParser(*this);
			archiveParser.Parse(readString,false);

			while(ifs){
				ifs.read(buffer,sizeof(buffer));
				readString=transformations[1]->ForceWrite(transformations[0]->ForceWrite(std::string(buffer,ifs.gcount())));
				archiveParser.Parse(readString,false);
			}

			readString=transformations[1]->ForceWriteAndClose(transformations[0]->ForceWriteAndClose(""));
			archiveParser.Parse(readString,true);
#endif
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

	if (!std::filesystem::exists(filename)){
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
	return PartialFileRead();
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

	std::string uncompressedContent=globals_.options_.archiveCompression_.get()->OpenForceWriteAndClose(compressedContent);

	return IsPaddingTrailer(uncompressedContent);
}

std::string XmltarArchive::ArchiveHeader(std::string filename, int archive_sequence_number){
    std::string s;

    s+="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"+globals_.options_.Newline();
    s+="<xmltar xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">"+globals_.options_.Newline();
    s+=globals_.options_.Tabs("\t")+"<archive-name>"+filename+"</archive-name>"+globals_.options_.Newline();
    s+=globals_.options_.Tabs("\t")+"<members>"+globals_.options_.Newline();

    return s;
}

std::string XmltarArchive::CompressedArchiveHeader(std::string filename, int archive_sequence_number){
	return globals_.options_.archiveCompression_.get()->OpenForceWriteAndClose(
						ArchiveHeader(filename, archive_sequence_number)
					);
}

std::string XmltarArchive::ArchiveTrailerBegin(bool lastMember){
	std::string lastMemberString;
	if (lastMember) lastMemberString="yea";
	else lastMemberString="nay";

    std::string s
		=globals_.options_.Tabs("\t")+"</members>"+globals_.options_.Newline()
		+globals_.options_.Tabs("\t")+"<padding last-member=\""+lastMemberString+"\">";

    return s;
}

std::string XmltarArchive::ArchiveTrailerMiddle(unsigned int padding){
    return std::string();
}

std::string XmltarArchive::ArchiveTrailerEnd(){
    std::string s="</padding>"+globals_.options_.Newline()+"</xmltar>"+globals_.options_.Newline();

    return s;
}

std::string XmltarArchive::CompressedArchiveTrailer(){
	std::string compressedArchiveTrailer
			=globals_.options_.archiveRawCompression_.get()->OpenForceWriteAndClose(
								ArchiveTrailerBegin(true)+ArchiveTrailerEnd()
			);

	return compressedArchiveTrailer;
}

std::string XmltarArchive::CompressedArchiveTrailer(unsigned int desiredLength){
	size_t desiredMemberLength
		=globals_.options_.archiveRawCompression_->MinimumPlaintextSizeGivenCompressedtextSize(desiredLength);

	size_t paddingLength=desiredMemberLength-ArchiveTrailerBegin(false).size()-ArchiveTrailerEnd().size();

	std::string data=ArchiveTrailerBegin(false)+std::string(paddingLength,' ')+ArchiveTrailerEnd();

	std::string archiveData=globals_.options_.archiveRawCompression_->GenerateCompressedText(desiredLength, data);

	return archiveData;
}
