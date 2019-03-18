/*
 * XmltarArchiveHandler.cpp
 *
 *  Created on: Jan 7, 2019
 *      Author: dbetz
 */

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <filesystem>

#include "Xmltar/XmlHandler.hpp"
#include "Xmltar/XmltarArchive.hpp"

extern "C" void XMLCALL StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts){
	((XmlHandler *) userData)->startElement(name,atts);
}

extern "C" void XMLCALL EndElementHandler(void *userData, const XML_Char *name){
	((XmlHandler *) userData)->endElement(name);
}

extern "C" void XMLCALL CharacterDataHandler(void *userData, XML_Char const *s, int len){
	((XmlHandler *) userData)->characterData(s,len);
}

XmlHandler::XmlHandler(){
	parser_ = XML_ParserCreate(NULL);
	XML_SetUserData(parser_, this);
    XML_SetElementHandler(parser_, StartElementHandler, EndElementHandler);
    XML_SetCharacterDataHandler(parser_, CharacterDataHandler);
}

