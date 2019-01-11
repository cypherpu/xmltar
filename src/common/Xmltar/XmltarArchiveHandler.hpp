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

#include <expat.h>

class XmltarArchive;

class XmltarArchiveHandler
{
	class Element {
	public:
		class Attribute {
		public:
			std::string name_;
			std::string value_;

			Attribute(char const *name, char const *value)
				: name_(name), value_(value) {}
		};
		std::string name_;
		std::vector<Attribute> attributes_;
		std::string characterData_;

		Element(const XML_Char *name, const XML_Char **atts)
			: name_(name) {
			for(size_t i=0; atts[i]!=nullptr; i+=2)
				attributes_.push_back(Attribute(atts[i],atts[i+1]));
		}
	};

	XmltarArchive & xmltarArchive_;
	std::vector<Element> elements_;
	XML_Parser parser_;
public:
	XmltarArchiveHandler(XmltarArchive & xmltarArchive);

	void Parse(std::string const & buff, bool done){
		std::cerr << "Parsing \"" << buff << "\"" << std::endl;
		if (XML_Parse(parser_, buff.c_str(), buff.size(), done) == XML_STATUS_ERROR) {
			std::cerr << "Error: " << XML_ErrorString(XML_GetErrorCode(parser_))
						<< " at " << XML_GetCurrentLineNumber(parser_) << std::endl;
			exit(-1);
		}
	}

	virtual ~XmltarArchiveHandler(){
		XML_ParserFree(parser_);
	}

	void startElement(const XML_Char *name, const XML_Char **atts);
	void endElement(const XML_Char *name);
	void characterData(XML_Char const *s, int len);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVEHANDLER_HPP_ */
