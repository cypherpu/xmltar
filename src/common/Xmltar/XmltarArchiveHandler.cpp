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

void XmltarArchiveHandler::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs){
	char* name = xercesc::XMLString::transcode(localname);
	elementNameStack_.push_back(name);
	characterDataStack_.push_back(std::vector<XMLCh>());
	if (elementNameStack_.back()=="file"){
		// snapshot_.fileEntries_.push_back(SnapshotFileEntry());
	}
	xercesc::XMLString::release(&name);
}

void XmltarArchiveHandler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname){
	char* name = xercesc::XMLString::transcode(localname);
	if (elementNameStack_.back()!=name){
		std::cerr << "end element sequence error " << elementNameStack_.back() << " " << name << std::endl;
		exit(1);
	}

	characterDataStack_.back().push_back(0);
	char* tmp = xercesc::XMLString::transcode(& characterDataStack_.back()[0]);
	std::string tmpStr(tmp);
	xercesc::XMLString::release(&tmp);

	if (elementNameStack_.back()=="option"){
		optionsStack_.push_back(tmpStr);
	}
	else if (elementNameStack_.back()=="path"){
		//snapshot_.fileEntries_.back().pathname_=tmpStr;
		//std::cerr << elementNameStack_.back() << "=\"" << tmpStr << "\"" << std::endl;
	}
	else if (elementNameStack_.back()=="last-backup-epoch-time"){
		//snapshot_.fileEntries_.back().lastBackupEpochTime_=boost::lexical_cast<time_t>(tmpStr);
		std::cerr << elementNameStack_.back() << "=\"" << tmpStr << "\"" << std::endl;
	}
	else if (elementNameStack_.back()=="last-backup-ascii-time"){
		//snapshot_.fileEntries_.back().lastBackupAsciiTime_=tmpStr;
		std::cerr << elementNameStack_.back() << "=\"" << tmpStr << "\"" << std::endl;
	}
	else if (elementNameStack_.back()=="volume-number"){
		//snapshot_.fileEntries_.back().lastBackupEpochTime_=boost::lexical_cast<unsigned int>(tmpStr);
		std::cerr << elementNameStack_.back() << "=\"" << tmpStr << "\"" << std::endl;
	}

	characterDataStack_.pop_back();
	elementNameStack_.pop_back();
	xercesc::XMLString::release(&name);
}

void XmltarArchiveHandler::fatalError(const xercesc::SAXParseException& exception){
	char* message = xercesc::XMLString::transcode(exception.getMessage());
	std::cout << "Fatal Error: " << message
		 << " at line: " << exception.getLineNumber()
		 << std::endl;
	xercesc::XMLString::release(&message);
}

void XmltarArchiveHandler::characters(const XMLCh* const chars, const XMLSize_t length){
	auto oldSize=characterDataStack_.back().size();
	characterDataStack_.back().resize(oldSize+length);
	std::copy(chars,chars+length,characterDataStack_.back().begin()+oldSize);
}

std::vector<char const *> XmltarArchiveHandler::optionsArgv(){
	std::vector<char const *> result;

	result.push_back("./xmltar");
	for(auto & i : optionsStack_)
		result.push_back(i.c_str());

	return result;
}
