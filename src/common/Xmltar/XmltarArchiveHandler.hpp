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
#include <map>

#include <expat.h>

class XmltarArchive;

class XmltarArchiveHandler
{
	class Element {
	public:
		std::string name_;
		std::map<std::string,std::string> attributes_;
		std::string characterData_;

		Element(const XML_Char *name, const XML_Char **atts)
			: name_(name) {
			for(size_t i=0; atts[i]!=nullptr; i+=2)
				if (attributes_.find(atts[i])==attributes_.end())
					attributes_[atts[i]]=atts[i+1];
				else throw std::logic_error("Element::Element: identical attribute names");
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
