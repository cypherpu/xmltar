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

	if (elements_.back().name_=="file")
		if (elements_.size()!=3) throw std::domain_error("XmltarArchiveHandler::startElement: \"file\" wrong nesting level");
		else if (elements_.end()[-2].name_!="members") throw std::domain_error("XmltarArchiveHandler::startElement \"members\" not parent of \"file\"");
		else if (elements_.back().attributes_.find("name")==elements_.back().attributes_.end())
			throw std::domain_error("XmltarArchiveHandler::startElement: \"file\" missing name attribute");

	if (elements_.back().name_=="content")
		if (elements_.size()!=4) throw std::domain_error("XmltarArchiveHandler::startElement: \"content\" wrong nesting level");
		else if (elements_.end()[-2].name_!="file") throw std::domain_error("XmltarArchiveHandler::startElement \"file\" not parent of \"content\"");

	if (elements_.back().name_=="content")
		std::filesystem::create_directories(elements_.end()[-2].attributes_["name"]);

	std::cerr << std::string('\t',elements_.size()) << "<" << name << ">" << std::endl;
}

void XmltarArchiveHandler::endElement(const XML_Char *name){
	std::cerr << std::string('\t',elements_.size()) << "</" << name << ">" << std::endl;

	elements_.pop_back();
}

void XmltarArchiveHandler::characterData(XML_Char const *s, int len){
}
