/*
 * XmlHandler.hpp
 *
 *  Created on: Jan 7, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLHANDLER_HPP_
#define SRC_COMMON_XMLTAR_XMLHANDLER_HPP_

#include <string>
#include <vector>
#include <map>

#include <expat.h>

class XmlHandler
{
	XML_Parser parser_;
public:
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

	std::vector<Element> elements_;

	XmlHandler();

	void Parse(XML_Char *buffer, size_t count, bool done){
		//std::cerr << "Parsing \"" << std::string(buffer,count) << "\"" << std::endl;
		if (XML_Parse(parser_, buffer, count, done) == XML_STATUS_ERROR) {
			std::cerr << "Error: " << XML_ErrorString(XML_GetErrorCode(parser_))
						<< " at line=" << XML_GetCurrentLineNumber(parser_) << " character=" << XML_GetCurrentColumnNumber(parser_) << std::endl;
			exit(-1);
		}
	}

	void Parse(std::string const & buff, bool done){
		//std::cerr << "Parsing \"" << buff << "\"" << std::endl;
		if (XML_Parse(parser_, buff.c_str(), buff.size(), done) == XML_STATUS_ERROR) {
			std::cerr << "Error: " << XML_ErrorString(XML_GetErrorCode(parser_))
						<< " at line=" << XML_GetCurrentLineNumber(parser_) << " character=" << XML_GetCurrentColumnNumber(parser_) << std::endl;
			exit(-1);
		}
	}

	virtual ~XmlHandler(){
		XML_ParserFree(parser_);
	}

	void startElementSetup(const XML_Char *name, const XML_Char **atts){
		elements_.push_back(Element(name,atts));
		startElement(name,atts);
	}
	void endElementTeardown(const XML_Char *name){
		endElement(name);
		elements_.pop_back();
	}
	void characterDataSetup(XML_Char const *s, int len){
		characterData(s,len);
	}

	virtual void startElement(const XML_Char *name, const XML_Char **atts){}
	virtual void endElement(const XML_Char *name){}
	virtual void characterData(XML_Char const *s, int len){}
};

#endif /* SRC_COMMON_XMLTAR_XMLHANDLER_HPP_ */
