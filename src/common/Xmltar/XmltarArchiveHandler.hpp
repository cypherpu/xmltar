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

extern "C" {
#include <expat.h>

static void XMLCALL startElementCallback(void *userData, const XML_Char *name, const XML_Char **atts);
static void XMLCALL endElementCallback(void *userData, const XML_Char *name);
}

class XmltarArchive;

class XmltarArchiveHandler
{
	XmltarArchive & xmltarArchive_;
	std::vector<std::string> elementNameStack_;
	std::vector<std::string> characterDataStack_;

	std::vector<std::string> optionsStack_;
public:
	XmltarArchiveHandler(XmltarArchive & xmltarArchive)
		: xmltarArchive_(xmltarArchive){}
	virtual ~XmltarArchiveHandler(){}

	void startElement(std::string const & name, std::vector<std::string> const & attributes);
	void endElement(std::string const & name);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVEHANDLER_HPP_ */
