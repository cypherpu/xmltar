/*
 * XmltarArchiveHandler.hpp
 *
 *  Created on: Jan 7, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVEHANDLER_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVEHANDLER_HPP_

#include <string>
#include <vector>

#include <xercesc/sax2/DefaultHandler.hpp>

class XmltarArchive;

class XmltarArchiveHandler : public xercesc::DefaultHandler
{
	XmltarArchive & xmltarArchive_;
	std::vector<std::string> elementNameStack_;
	std::vector<std::vector<XMLCh>> characterDataStack_;

	std::vector<std::string> optionsStack_;
public:
	XmltarArchiveHandler(XmltarArchive & xmltarArchive)
		: xmltarArchive_(xmltarArchive){}
	virtual ~XmltarArchiveHandler(){}

	void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs);
	void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
	void characters(const XMLCh* const chars, const XMLSize_t length);
	void fatalError(const xercesc::SAXParseException&);

	std::vector<char const *> optionsArgv();
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVEHANDLER_HPP_ */
