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

void XmltarArchiveHandler::startElement(std::string const & name, std::vector<std::string> const & attributes){
	elementNameStack_.push_back(name);
	characterDataStack_.push_back("");
}

void XmltarArchiveHandler::endElement(std::string const & name){
	characterDataStack_.pop_back();
	elementNameStack_.pop_back();
}

void XmltarArchiveHandler::characters(std::string const & chars){
	characterDataStack_.back()+=chars;
}
