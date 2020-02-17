/*
 * XmltarArchiveExtractMultiVolume.hpp
 *
 *  Created on: Mar 16, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTMULTIVOLUME_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTMULTIVOLUME_HPP_

#include "Xmltar/XmltarArchive.hpp"
#include "XmlParser/XmlParser.hpp"

class XmltarArchiveExtractMultiVolume;

class XmltarMultiVolumeXmlHandler : public XmlParser {
	XmltarArchiveExtractMultiVolume & xmltarArchiveExtractMultiVolume_;
	bool firstDecodedLine_;
	bool encounteredTrailingTabs_;
public:
	XmltarMultiVolumeXmlHandler(XmltarArchiveExtractMultiVolume & xmltarArchiveExtractMultiVolume)
		: xmltarArchiveExtractMultiVolume_(xmltarArchiveExtractMultiVolume), firstDecodedLine_(false), encounteredTrailingTabs_(false){
	}

	virtual void startElement(const XML_Char *name, const XML_Char **atts) override;
	virtual void endElement(const XML_Char *name) override;
	virtual void characterData(XML_Char const *s, int len) override;
};

class XmltarArchiveExtractMultiVolume : public XmltarArchive {
public:
	std::fstream fs_;

public:
	XmltarArchiveExtractMultiVolume(
		XmltarGlobals & globals,
		std::string filename
	);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTMULTIVOLUME_HPP_ */
