/*

XmltarArchiveExtractMultiVolume.cpp
Copyright 2019-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: Mar 16, 2019
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

#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "Utilities/XMLSafeString.hpp"
#include "Xmltar/XmltarArchiveExtractMultiVolume.hpp"

void XmltarMultiVolumeXmlHandler::startElement(const XML_Char *name, const XML_Char **atts){
	if (elements_.back().name_=="xmltar"){
		if (elements_.size()!=1) throw std::domain_error("XmltarArchiveHandler::startElement: \"xmltar\" wrong nesting level");
	}
	else if (elements_.back().name_=="members"){
		if (elements_.size()!=2) throw std::domain_error("XmltarArchiveHandler::startElement: \"members\" wrong nesting level");
	}
	else if (elements_.back().name_=="file"){
		if (elements_.size()!=3) throw std::domain_error("XmltarArchiveHandler::startElement: \"file\" wrong nesting level");
		else if (elements_.end()[-2].name_!="members") throw std::domain_error("XmltarArchiveHandler::startElement \"members\" not parent of \"file\"");
	}
	else if (elements_.back().name_=="content"){
		if (elements_.size()!=4) throw std::domain_error("XmltarArchiveHandler::startElement: \"content\" wrong nesting level");
		else if (elements_.end()[-2].name_!="file") throw std::domain_error("XmltarArchiveHandler::startElement \"file\" not parent of \"content\"");

		if (elements_.back().attributes_.at("type")=="symlink")
			symlink(DecodeXMLSafeStringToString(elements_.back().attributes_.at("target")).c_str(),
					elements_.end()[-2].attributes_.at("name").c_str());
	}
	else if (elements_.back().name_=="stream"){
		if (elements_.size()!=5) throw std::domain_error("XmltarArchiveHandler::startElement: \"stream\" wrong nesting level");
		else if (elements_.end()[-2].name_!="content") throw std::domain_error("XmltarArchiveHandler::startElement \"content\" not parent of \"stream\"");
	}

	if (elements_.back().name_=="content" && elements_.back().attributes_.at("type")=="directory")
			std::filesystem::create_directories(elements_.end()[-2].attributes_.at("name"));
	else if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		// create directories leading to file
		//std::cerr << "XmltarMultiVolumeXmlHandler::startElement: before create directories" << std::endl;
		std::filesystem::path p=std::filesystem::path(elements_.end()[-3].attributes_.at("name"));
		if (p.has_parent_path())
			std::filesystem::create_directories(p.parent_path());
		// FIXME - investigate file open flags
		if (boost::lexical_cast<std::streamoff>(elements_.back().attributes_.at("this-extent-start"))==0)
			xmltarArchiveExtractMultiVolume_.fs_.open(DecodeXMLSafeStringToString(elements_.end()[-3].attributes_.at("name")),std::fstream::out|std::fstream::trunc);
		else
			xmltarArchiveExtractMultiVolume_.fs_.open(DecodeXMLSafeStringToString(elements_.end()[-3].attributes_.at("name")),std::ios::in|std::ios::out);
		xmltarArchiveExtractMultiVolume_.fs_.seekp(boost::lexical_cast<std::streamoff>(elements_.back().attributes_.at("this-extent-start")),std::ios_base::beg);

		// xmltarArchiveExtractMultiVolume_.decoder_.reset(xmltarArchiveExtractMultiVolume_.globals_.options_.encoding_->clone());
		xmltarArchiveExtractMultiVolume_.globals_.options_.decoding_->Open();

		// xmltarArchiveExtractMultiVolume_.fileDecompression_.reset(xmltarArchiveExtractMultiVolume_.globals_.options_.fileCompression_->clone());
		xmltarArchiveExtractMultiVolume_.globals_.options_.fileDecompression_->Open();
		firstDecodedLine_=true;
		encounteredTrailingTabs_=false;
	}
}

void XmltarMultiVolumeXmlHandler::endElement(const XML_Char *name){
	if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		xmltarArchiveExtractMultiVolume_.fs_ << xmltarArchiveExtractMultiVolume_.globals_.options_.fileDecompression_->ForceWriteAndClose(
			xmltarArchiveExtractMultiVolume_.globals_.options_.decoding_->ForceWriteAndClose("")
		);
		xmltarArchiveExtractMultiVolume_.fs_.close();
	}
}

void XmltarMultiVolumeXmlHandler::characterData(XML_Char const *s, int len){
	if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		if (encounteredTrailingTabs_){
			while(len>0 && s[len-1]=='\t')
					--len;
			if (len>0)
				throw std::logic_error("non-tab character in trailing tabs");
		}

		if (len>0 && s[len-1]=='\t'){
			encounteredTrailingTabs_=true;
			while(len>0 && s[len-1]=='\t')
					--len;
		}
		if (firstDecodedLine_ && len>=1 && s[0]=='\n')
			xmltarArchiveExtractMultiVolume_.fs_ << xmltarArchiveExtractMultiVolume_.globals_.options_.fileDecompression_->ForceWrite(xmltarArchiveExtractMultiVolume_.globals_.options_.decoding_->ForceWrite(std::string(s+1,len-1)));
		else
			xmltarArchiveExtractMultiVolume_.fs_ << xmltarArchiveExtractMultiVolume_.globals_.options_.fileDecompression_->ForceWrite(xmltarArchiveExtractMultiVolume_.globals_.options_.decoding_->ForceWrite(std::string(s,len)));

		firstDecodedLine_=false;
	}
}

XmltarArchiveExtractMultiVolume::XmltarArchiveExtractMultiVolume(XmltarGlobals & globals, std::string filename)
	: ::XmltarArchive(globals,filename)
{
	XmltarMultiVolumeXmlHandler xmltarMultiVolumeHandler(*this);

	std::ifstream ifs(filename);

	XML_Char buffer[1024];

	globals_.options_.archiveDecompression_->Open();

	std::string tmp;
	tmp+=globals_.options_.archiveDecompression_->ForceWrite(
			globals_.options_.archiveDecryption_->Open(
						globals_.key_
					)
			);
	while(ifs){
		ifs.read(buffer,sizeof(buffer)/sizeof(*buffer));

		tmp+=globals_.options_.archiveDecompression_->ForceWrite(
				globals_.options_.archiveDecryption_->Decrypt(
						std::string(buffer,ifs.gcount())
				)
			);
		if (!xmltarMultiVolumeHandler.Parse(tmp,false)){
			exit(-1);
		}

		tmp.clear();
	}

	tmp=globals_.options_.archiveDecompression_->ForceWriteAndClose(
			globals_.options_.archiveDecryption_->Close()
		);
	if (!xmltarMultiVolumeHandler.Parse(tmp,false)){
		exit(-1);
	}

}


