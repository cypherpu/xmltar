/*
 * XmltarArchiveExtractSingleVolume.hpp
 *
 *  Created on: Mar 16, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTSINGLEVOLUME_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTSINGLEVOLUME_HPP_

#include "Xmltar/XmltarArchive.hpp"
#include "XmlParser/XmlParser.hpp"

class XmltarArchiveExtractSingleVolume;

class XmltarSingleVolumeXmlHandler : public XmlParser {
	XmltarArchiveExtractSingleVolume & xmltarArchiveExtractSingleVolume_;
public:
	XmltarSingleVolumeXmlHandler(XmltarArchiveExtractSingleVolume & xmltarArchiveExtractSingleVolume)
		: xmltarArchiveExtractSingleVolume_(xmltarArchiveExtractSingleVolume){
	}

	virtual void startElement(const XML_Char *name, const XML_Char **atts) override;
	virtual void endElement(const XML_Char *name) override;
	virtual void characterData(XML_Char const *s, int len) override;
};

class XmltarArchiveExtractSingleVolume : public XmltarArchive {
public:
	std::unique_ptr<CompressorInterface> decoder_;
	std::unique_ptr<CompressorInterface> fileDecompression_;
	std::fstream fs_;

public:
	XmltarArchiveExtractSingleVolume(
		XmltarOptions const & opts,
		XmltarGlobals & globals,
		std::string filename,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTSINGLEVOLUME_HPP_ */
