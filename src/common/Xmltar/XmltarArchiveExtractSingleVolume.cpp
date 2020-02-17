/*
 * XmltarArchiveExtractSingleVolume.cpp
 *
 *  Created on: Mar 16, 2019
 *      Author: dbetz
 */

#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "Xmltar/XmltarArchiveExtractSingleVolume.hpp"

void XmltarSingleVolumeXmlHandler::startElement(const XML_Char *name, const XML_Char **atts){
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
			xmltarArchiveExtractSingleVolume_.fs_.open(elements_.end()[-3].attributes_.at("name"),std::fstream::out|std::fstream::trunc);
		else
			xmltarArchiveExtractSingleVolume_.fs_.open(elements_.end()[-3].attributes_.at("name"),std::ios::in|std::ios::out);
		std::cerr << std::string(elements_.size(),'\t') << "boost::lexical_cast<std::streamoff>(elements_.back().attributes_.at(\"this-extent-start\"))=" << boost::lexical_cast<std::streamoff>(elements_.back().attributes_.at("this-extent-start")) << std::endl;
		xmltarArchiveExtractSingleVolume_.fs_.seekp(boost::lexical_cast<std::streamoff>(elements_.back().attributes_.at("this-extent-start")),std::ios_base::beg);

		// xmltarArchiveExtractSingleVolume_.decoder_.reset(xmltarArchiveExtractSingleVolume_.globals_.options_.encoding_->clone());
		xmltarArchiveExtractSingleVolume_.globals_.options_.decoding_->Open();

		//xmltarArchiveExtractSingleVolume_.fileDecompression_.reset(xmltarArchiveExtractSingleVolume_.globals_.options_.fileCompression_->clone());
		xmltarArchiveExtractSingleVolume_.globals_.options_.fileDecompression_->Open();
		firstDecodedLine_=true;
		encounteredTrailingTabs_=false;
	}

	std::cerr << std::string(elements_.size(),'\t') << "<" << name << ">" << std::endl;
}

void XmltarSingleVolumeXmlHandler::endElement(const XML_Char *name){
	std::cerr << std::string(elements_.size(),'\t') << "</" << name << ">" << std::endl;

	if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		xmltarArchiveExtractSingleVolume_.fs_ << xmltarArchiveExtractSingleVolume_.globals_.options_.fileDecompression_->ForceWriteAndClose(xmltarArchiveExtractSingleVolume_.globals_.options_.decoding_->ForceWriteAndClose(""));
		xmltarArchiveExtractSingleVolume_.fs_.close();
	}

}

void XmltarSingleVolumeXmlHandler::characterData(XML_Char const *s, int len){
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
			xmltarArchiveExtractSingleVolume_.fs_ << xmltarArchiveExtractSingleVolume_.globals_.options_.fileDecompression_->ForceWrite(xmltarArchiveExtractSingleVolume_.globals_.options_.decoding_->ForceWrite(std::string(s+1,len-1)));
		else
			xmltarArchiveExtractSingleVolume_.fs_ << xmltarArchiveExtractSingleVolume_.globals_.options_.fileDecompression_->ForceWrite(xmltarArchiveExtractSingleVolume_.globals_.options_.decoding_->ForceWrite(std::string(s,len)));

		firstDecodedLine_=false;
	}
}

XmltarArchiveExtractSingleVolume::XmltarArchiveExtractSingleVolume(XmltarGlobals & globals, std::string filename)
	: ::XmltarArchive(globals,filename)
{
	std::cerr << "XmltarArchiveExtractSingleVolume::XmltarSingleVolumeXmlHandler: entering: filename=" << filename << std::endl;

	XmltarSingleVolumeXmlHandler xmltarSingleVolumeHandler(*this);

	std::cerr << "XmltarArchiveExtractSingleVolume::XmltarSingleVolumeXmlHandler: created handler" << std::endl;

	std::ifstream ifs(filename);

	XML_Char buffer[1024];

	//std::shared_ptr<CompressorInterface> archiveDecompression(globals_.options_.archiveDecompression_->clone());
	//std::shared_ptr<CompressorInterface> memberDecompression(globals_.options_.archiveMemberDecompression_->clone());

	if (ifs)
		std::cerr << "XmltarArchiveExtractSingleVolume::XmltarSingleVolumeXmlHandler: opened fiiename" << std::endl;
	globals_.options_.archiveDecompression_->Open();
	std::cerr << "XmltarArchiveExtractSingleVolume::XmltarSingleVolumeXmlHandler: after archive decompression" << std::endl;
	globals_.options_.archiveMemberDecompression_->Open();

	std::cerr << "XmltarArchiveExtractSingleVolume::XmltarSingleVolumeXmlHandler: opened decompression" << std::endl;

	std::string tmp;
	while(ifs){
		ifs.read(buffer,sizeof(buffer)/sizeof(*buffer));

		tmp=globals_.options_.archiveMemberDecompression_->ForceWrite(globals_.options_.archiveDecompression_->ForceWrite(std::string(buffer,ifs.gcount())));
		//std::cerr << "ifs.gcount()=" << ifs.gcount() << std::endl;
		xmltarSingleVolumeHandler.Parse(tmp,false);
	}

	tmp=globals_.options_.archiveMemberDecompression_->ForceWriteAndClose(globals_.options_.archiveDecompression_->ForceWriteAndClose(""));
	//std::cerr << "ifs.gcount()=" << ifs.gcount() << std::endl;
	xmltarSingleVolumeHandler.Parse(tmp,false);
}


