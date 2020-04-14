/*

SnapshotXmlParser.hpp
Copyright 2017-2020 David A. Betz
David.Betz.MD@gmail.com
Created on: Nov 20, 2017
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

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTXMLPARSER_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTXMLPARSER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <deque>

class Snapshot;

#include "Snapshot/SnapshotFileEntry.hpp"
#include "XmlParser/XmlParser.hpp"

class SnapshotXmlParser : public XmlParser {
	SnapshotFileEntry tmp_;
	std::deque<SnapshotFileEntry> & fileEntries_;
public:
	SnapshotXmlParser(std::deque<SnapshotFileEntry> & fileEntries)
		: fileEntries_(fileEntries){
	}

	virtual void startElement(const XML_Char *name, const XML_Char **atts) override;
	virtual void endElement(const XML_Char *name) override;
	virtual void characterData(XML_Char const *s, int len) override;
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTXMLPARSER_HPP_ */
