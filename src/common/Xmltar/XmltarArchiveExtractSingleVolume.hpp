/*

XmltarArchiveExtractSingleVolume.hpp
Copyright 2017-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: Mar 16, 2019
Author: dbetz

This file is part of Xmltar.

Xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Xmltar.  If not, see <https://www.gnu.org/licenses/>.

 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTSINGLEVOLUME_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTSINGLEVOLUME_HPP_

#include "Xmltar/XmltarArchive.hpp"
#include "XmlParser/XmlParser.hpp"

class XmltarArchiveExtractSingleVolume;

class XmltarSingleVolumeXmlHandler : public XmlParser {
	XmltarArchiveExtractSingleVolume & xmltarArchiveExtractSingleVolume_;
	bool firstDecodedLine_;
	bool encounteredTrailingTabs_;
	bool skipRegularFile_;
public:
	XmltarSingleVolumeXmlHandler(XmltarArchiveExtractSingleVolume & xmltarArchiveExtractSingleVolume)
		: xmltarArchiveExtractSingleVolume_(xmltarArchiveExtractSingleVolume), firstDecodedLine_(false), encounteredTrailingTabs_(false){
	}

	virtual void startElement(const XML_Char *name, const XML_Char **atts) override;
	virtual void endElement(const XML_Char *name) override;
	virtual void characterData(XML_Char const *s, int len) override;
};

class XmltarArchiveExtractSingleVolume : public XmltarArchive {
public:
	std::fstream fs_;

public:
	XmltarArchiveExtractSingleVolume(
		XmltarGlobals & globals,
		std::string filename
	);

	bool Matches(std::string const & filename);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVEEXTRACTSINGLEVOLUME_HPP_ */
