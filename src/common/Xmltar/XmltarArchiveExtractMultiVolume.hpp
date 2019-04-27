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
		XmltarOptions const & opts,
		XmltarGlobals & globals,
		std::string filename,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTMULTIVOLUME_HPP_ */
