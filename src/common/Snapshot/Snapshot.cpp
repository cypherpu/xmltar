/*
 * Snapshot.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#include <iostream>

#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XMLString.hpp>

#include <boost/lexical_cast.hpp>

#include "Snapshot/Snapshot.hpp"

void Snapshot::SnapshotHandler::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs){
	char* name = xercesc::XMLString::transcode(localname);
	elementNameStack_.push_back(name);
	characterDataStack_.push_back(std::vector<XMLCh>());
	if (elementNameStack_.back()=="file"){
		snapshot_.fileEntries_.push_back(SnapshotFileEntry());
	}
	xercesc::XMLString::release(&name);
}

void Snapshot::SnapshotHandler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname){
	char* name = xercesc::XMLString::transcode(localname);
	if (elementNameStack_.back()!=name){
		std::cerr << "end element sequence error " << elementNameStack_.back() << " " << name << std::endl;
		exit(1);
	}

	characterDataStack_.back().push_back(0);
	char* tmp = xercesc::XMLString::transcode(& characterDataStack_.back()[0]);
	std::string tmpStr(tmp);
	xercesc::XMLString::release(&tmp);

	if (elementNameStack_.back()=="path"){
		snapshot_.fileEntries_.back().pathname_=tmpStr;
		std::cerr << elementNameStack_.back() << "=\"" << tmpStr << "\"" << std::endl;
	}
	else if (elementNameStack_.back()=="last-backup-epoch-time"){
		snapshot_.fileEntries_.back().lastBackupEpochTime_=boost::lexical_cast<time_t>(tmpStr);
		std::cerr << elementNameStack_.back() << "=\"" << tmpStr << "\"" << std::endl;
	}
	else if (elementNameStack_.back()=="last-backup-ascii-time"){
		snapshot_.fileEntries_.back().lastBackupAsciiTime_=tmpStr;
		std::cerr << elementNameStack_.back() << "=\"" << tmpStr << "\"" << std::endl;
	}
	else if (elementNameStack_.back()=="volume-number"){
		snapshot_.fileEntries_.back().lastBackupEpochTime_=boost::lexical_cast<unsigned int>(tmpStr);
		std::cerr << elementNameStack_.back() << "=\"" << tmpStr << "\"" << std::endl;
	}

	characterDataStack_.pop_back();
	elementNameStack_.pop_back();
	xercesc::XMLString::release(&name);
}

void Snapshot::SnapshotHandler::fatalError(const xercesc::SAXParseException& exception){
	char* message = xercesc::XMLString::transcode(exception.getMessage());
	std::cout << "Fatal Error: " << message
		 << " at line: " << exception.getLineNumber()
		 << std::endl;
	xercesc::XMLString::release(&message);
}

void Snapshot::SnapshotHandler::characters(const XMLCh* const chars, const XMLSize_t length){
	auto oldSize=characterDataStack_.back().size();
	characterDataStack_.back().resize(oldSize+length);
	std::copy(chars,chars+length,characterDataStack_.back().begin()+oldSize);
}

Snapshot::Snapshot(){
}

Snapshot::Snapshot(std::string const & xmlFile){
    try {
        xercesc::XMLPlatformUtils::Initialize();
    }
    catch (const xercesc::XMLException& toCatch) {
        char* message = xercesc::XMLString::transcode(toCatch.getMessage());
        std::cout << "Error during initialization! :\n";
        std::cout << "Exception message is: \n"
             << message << "\n";
        xercesc::XMLString::release(&message);
        //return 1;
    }

    xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
    parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional

    xercesc::DefaultHandler* defaultHandler = new SnapshotHandler(*this);

    parser->setContentHandler(defaultHandler);
    parser->setErrorHandler(defaultHandler);

    try {
        parser->parse(xmlFile.c_str());
    }
    catch (const xercesc::XMLException& toCatch) {
        char* message = xercesc::XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message is: \n"
             << message << "\n";
        xercesc::XMLString::release(&message);
        //return -1;
    }
    catch (const xercesc::SAXParseException& toCatch) {
        char* message = xercesc::XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message is: \n"
             << message << "\n";
        xercesc::XMLString::release(&message);
        //return -1;
    }
    catch (...) {
        std::cout << "Unexpected Exception \n" ;
        //return -1;
    }

    delete parser;
    delete defaultHandler;
    //return 0;
}
