/*
 * XmltarArchiveExtractMultiVolume.hpp
 *
 *  Created on: Mar 16, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTMULTIVOLUME_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTMULTIVOLUME_HPP_

#include "Xmltar/XmltarArchive.hpp"
#include "Xmltar/XmlHandler.hpp"

class XmltarArchiveExtractMultiVolume;

class XmltarMultiVolumeXmlHandler : public XmlHandler {
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

	XmltarArchiveExtractMultiVolume & xmltarArchiveExtractMultiVolume_;
public:
	XmltarMultiVolumeXmlHandler(XmltarArchiveExtractMultiVolume & xmltarArchiveExtractMultiVolume)
		: xmltarArchiveExtractMultiVolume_(xmltarArchiveExtractMultiVolume){
	}

	virtual void startElement(const XML_Char *name, const XML_Char **atts) override;
	virtual void endElement(const XML_Char *name) override;
	virtual void characterData(XML_Char const *s, int len) override;
};

class XmltarArchiveExtractMultiVolume : public XmltarArchive {
public:
	std::unique_ptr<Transform> decoder_;
	std::unique_ptr<Transform> fileDecompression_;
	std::fstream fs_;

public:
	XmltarArchiveExtractMultiVolume(
		XmltarOptions & opts,
		std::string filename,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTMULTIVOLUME_HPP_ */
