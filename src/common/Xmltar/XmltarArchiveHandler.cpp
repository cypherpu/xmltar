/*
 * XmltarArchiveHandler.cpp
 *
 *  Created on: Jan 7, 2019
 *      Author: dbetz
 */

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <filesystem>

#include "Xmltar/XmltarArchiveHandler.hpp"
#include "Xmltar/XmltarArchive.hpp"

extern "C" void XMLCALL StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts){
	((XmltarArchiveHandler *) userData)->startElement(name,atts);
}

extern "C" void XMLCALL EndElementHandler(void *userData, const XML_Char *name){
	((XmltarArchiveHandler *) userData)->endElement(name);
}

extern "C" void XMLCALL CharacterDataHandler(void *userData, XML_Char const *s, int len){
	((XmltarArchiveHandler *) userData)->characterData(s,len);
}

XmltarArchiveHandler::XmltarArchiveHandler(XmltarArchive & xmltarArchive)
	: xmltarArchive_(xmltarArchive){

	parser_ = XML_ParserCreate(NULL);
	XML_SetUserData(parser_, this);
    XML_SetElementHandler(parser_, StartElementHandler, EndElementHandler);
    XML_SetCharacterDataHandler(parser_, CharacterDataHandler);
}

void XmltarArchiveHandler::startElement(const XML_Char *name, const XML_Char **atts){
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
		xmltarArchive_.ofs_.open(elements_.end()[-3].attributes_.at("name"));
		if (elements_.back().attributes_.at("encoding")=="xxd") xmltarArchive_.decoder_.reset(new TransformHex("decoder"));
		xmltarArchive_.decoder_->OpenDecompression();
	}

	std::cerr << std::string('\t',elements_.size()) << "<" << name << ">" << std::endl;
}

void XmltarArchiveHandler::endElement(const XML_Char *name){
	std::cerr << std::string('\t',elements_.size()) << "</" << name << ">" << std::endl;

	if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		xmltarArchive_.ofs_ << xmltarArchive_.decoder_->Close();
		xmltarArchive_.ofs_.close();
	}

	elements_.pop_back();
}

void XmltarArchiveHandler::characterData(XML_Char const *s, int len){
	if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		xmltarArchive_.decoder_->Write(std::string(s,len));
		xmltarArchive_.ofs_ << xmltarArchive_.decoder_->Read();
	}
}
