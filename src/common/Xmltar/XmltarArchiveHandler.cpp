/*
 * XmltarArchiveHandler.cpp
 *
 *  Created on: Jan 7, 2019
 *      Author: dbetz
 */

#include <iostream>

#include <boost/lexical_cast.hpp>

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
	elementNameStack_.push_back(name);
	characterDataStack_.push_back("");

	std::cerr << std::string('\t',elementNameStack_.size()) << "<" << name << ">" << std::endl;
}

void XmltarArchiveHandler::endElement(const XML_Char *name){
	std::cerr << std::string('\t',elementNameStack_.size()) << "</" << name << ">" << std::endl;

	characterDataStack_.pop_back();
	elementNameStack_.pop_back();
}

void XmltarArchiveHandler::characterData(XML_Char const *s, int len){
}
