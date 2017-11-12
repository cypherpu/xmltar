/*
 * SnapshotParser.hpp
 *
 *  Created on: Nov 5, 2017
 *      Author: dbetz
 */

#include <iostream>

#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XMLString.hpp>

#include "Snapshot/Snapshot.hpp"

class SnapshotHandler : public xercesc::DefaultHandler
{
	Snapshot & snapshot_;
	std::vector<std::string> elementNameStack_;
	std::vector<std::vector<XMLCh>> characterDataStack_;
public:
	SnapshotHandler(Snapshot & snapshot)
		: snapshot_(snapshot){}
	virtual ~SnapshotHandler(){}

	void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs);
	void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
	void characters(const XMLCh* const chars, const XMLSize_t length);
	void fatalError(const xercesc::SAXParseException&);
};

void SnapshotHandler::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs){
	char* name = xercesc::XMLString::transcode(localname);
	elementNameStack_.push_back(name);
	characterDataStack_.push_back(std::vector<XMLCh>());
	if (elementNameStack_.back()=="file"){
		snapshot_.fileEntries_.push_back(SnapshotFileEntry());
	}
	xercesc::XMLString::release(&name);
}

void SnapshotHandler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname){
	char* name = xercesc::XMLString::transcode(localname);
	if (elementNameStack_.back()!=name){
		std::cerr << "end element sequence error " << elementNameStack_.back() << " " << name << std::endl;
		exit(1);
	}

	characterDataStack_.back().push_back(0);
	if (elementNameStack_.back()=="path"){
		char* tmp = xercesc::XMLString::transcode(& characterDataStack_.back()[0]);
		std::cerr << elementNameStack_.back() << "=\"" << tmp << "\"" << std::endl;
		xercesc::XMLString::release(&name);
	}
	else if (elementNameStack_.back()=="last-backup-epoch-time"){
		char* tmp = xercesc::XMLString::transcode(& characterDataStack_.back()[0]);
		std::cerr << elementNameStack_.back() << "=\"" << tmp << "\"" << std::endl;
		xercesc::XMLString::release(&name);
	}
	else if (elementNameStack_.back()=="last-backup-ascii-time"){
		char* tmp = xercesc::XMLString::transcode(& characterDataStack_.back()[0]);
		std::cerr << elementNameStack_.back() << "=\"" << tmp << "\"" << std::endl;
		xercesc::XMLString::release(&name);
	}
	else if (elementNameStack_.back()=="volume-number"){
		char* tmp = xercesc::XMLString::transcode(& characterDataStack_.back()[0]);
		std::cerr << elementNameStack_.back() << "=\"" << tmp << "\"" << std::endl;
		xercesc::XMLString::release(&name);
	}

	characterDataStack_.pop_back();
	elementNameStack_.pop_back();
	xercesc::XMLString::release(&name);
}

void SnapshotHandler::fatalError(const xercesc::SAXParseException& exception){
	char* message = xercesc::XMLString::transcode(exception.getMessage());
	std::cout << "Fatal Error: " << message
		 << " at line: " << exception.getLineNumber()
		 << std::endl;
	xercesc::XMLString::release(&message);
}

void SnapshotHandler::characters(const XMLCh* const chars, const XMLSize_t length){
	auto oldSize=characterDataStack_.back().size();
	characterDataStack_.back().resize(oldSize+length);
	std::copy(chars,chars+length,characterDataStack_.back().begin()+oldSize);
}

bool ParseSnapshot(std::string input, Snapshot & snapshot){
    try {
        xercesc::XMLPlatformUtils::Initialize();
    }
    catch (const xercesc::XMLException& toCatch) {
        char* message = xercesc::XMLString::transcode(toCatch.getMessage());
        std::cout << "Error during initialization! :\n";
        std::cout << "Exception message is: \n"
             << message << "\n";
        xercesc::XMLString::release(&message);
        return 1;
    }

    char const * xmlFile = "snapshot-template.xml";
    xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
    parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional

    xercesc::DefaultHandler* defaultHandler = new SnapshotHandler(snapshot);

    parser->setContentHandler(defaultHandler);
    parser->setErrorHandler(defaultHandler);

    try {
        parser->parse(xmlFile);
    }
    catch (const xercesc::XMLException& toCatch) {
        char* message = xercesc::XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message is: \n"
             << message << "\n";
        xercesc::XMLString::release(&message);
        return -1;
    }
    catch (const xercesc::SAXParseException& toCatch) {
        char* message = xercesc::XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message is: \n"
             << message << "\n";
        xercesc::XMLString::release(&message);
        return -1;
    }
    catch (...) {
        std::cout << "Unexpected Exception \n" ;
        return -1;
    }

    delete parser;
    delete defaultHandler;
    return 0;
}
