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

#include "Snapshot/Snapshot.hpp"
#include "Snapshot/SnapshotHandler.hpp"

Snapshot::Snapshot(){
}

Snapshot::Snapshot(std::string const & xmlFile){
	Load(xmlFile);
}

void Snapshot::Load(std::string const & xmlFile){
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

    SnapshotHandler* defaultHandler = new SnapshotHandler(*this);

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

    std::vector<char const *> argv=defaultHandler->optionsArgv();
    options_.ProcessOptions(argv.size(),&argv[0]);

    delete parser;
    delete defaultHandler;
    //return 0;
}
