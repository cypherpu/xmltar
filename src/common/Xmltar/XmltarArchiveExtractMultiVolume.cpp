/*
 * XmltarArchiveExtractMultiVolume.cpp
 *
 *  Created on: Mar 16, 2019
 *      Author: dbetz
 */

#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "Xmltar/XmltarArchiveExtractMultiVolume.hpp"

void XmltarMultiVolumeXmlHandler::startElement(const XML_Char *name, const XML_Char **atts){
	elements_.push_back(Element(name,atts));

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
		// FIXME - create directories leading to file
		xmltarArchiveExtractMultiVolume_.ofs_.open(elements_.end()[-3].attributes_.at("name"));
		xmltarArchiveExtractMultiVolume_.ofs_.seekp(boost::lexical_cast<std::streamoff>(elements_.back().attributes_.at("this-extent-start")),std::ios_base::beg);
		if (elements_.back().attributes_.at("encoding")=="xxd") xmltarArchiveExtractMultiVolume_.decoder_.reset(new TransformHex("decoder"));
		xmltarArchiveExtractMultiVolume_.decoder_->OpenDecompression();
	}

	std::cerr << std::string('\t',elements_.size()) << "<" << name << ">" << std::endl;
}

void XmltarMultiVolumeXmlHandler::endElement(const XML_Char *name){
	std::cerr << std::string('\t',elements_.size()) << "</" << name << ">" << std::endl;

	if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		xmltarArchiveExtractMultiVolume_.ofs_ << xmltarArchiveExtractMultiVolume_.decoder_->ForceWriteAndClose("");
		xmltarArchiveExtractMultiVolume_.ofs_.close();
	}

	elements_.pop_back();
}

void XmltarMultiVolumeXmlHandler::characterData(XML_Char const *s, int len){
	if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		xmltarArchiveExtractMultiVolume_.ofs_ << xmltarArchiveExtractMultiVolume_.decoder_->ForceWrite(std::string(s,len));
	}
}

XmltarArchiveExtractMultiVolume::XmltarArchiveExtractMultiVolume(XmltarOptions & opts, std::string filename, std::shared_ptr<XmltarMemberCreate> & nextMember)
	: ::XmltarArchive(opts,filename,nextMember_)
{
	std::cerr << "XmltarArchiveExtractMultiVolume::XmltarArchiveExtractMultiVolume: entering: filename=" << filename << std::endl;


	XmltarMultiVolumeXmlHandler xmltarMultiVolumeHandler(*this);

	std::ifstream ifs(filename);

	XML_Char buffer[1024];
	while(ifs){
		ifs.read(buffer,sizeof(buffer)/sizeof(*buffer));
		std::cerr << "ifs.gcount()=" << ifs.gcount() << std::endl;
		xmltarMultiVolumeHandler.Parse(buffer,ifs.gcount(),false);
	}
}


