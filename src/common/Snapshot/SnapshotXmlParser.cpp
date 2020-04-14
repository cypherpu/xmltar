/*

SnapshotXmlParser.cpp
Copyright 2019-2020 David A. Betz
David.Betz.MD@gmail.com
Created on: Apr 21, 2019
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

#include "Snapshot/SnapshotEvent.hpp"
#include "Snapshot/SnapshotXmlParser.hpp"
#include "Utilities/XMLSafeString.hpp"

void SnapshotXmlParser::startElement(const XML_Char *name, const XML_Char **atts){
	if (elements_.back().name_=="snapshot"){
		if (elements_.size()!=1) throw std::domain_error("SnapshotXmlParser::startElement: \"snapshot\" wrong nesting level");
	}
	else if (elements_.back().name_=="file"){
		if (elements_.size()!=2) throw std::domain_error("SnapshotXmlParser::startElement: \"file\" wrong nesting level");
		else if (elements_.end()[-2].name_!="snapshot") throw std::domain_error("SnapshotXmlParser::endElement \"snapshot\" not parent of \"file\"");

		std::filesystem::path filePath(DecodeXMLSafeStringToString(elements_.end()[-1].attributes_["name"]));
		tmp_=SnapshotFileEntry(ExtendedPath(filePath));
		std::cerr << "file name=" << filePath << " ";
	}
	else if (elements_.back().name_=="event"){
		if (elements_.size()!=3) throw std::domain_error("SnapshotXmlParser::startElement: \"event\" wrong nesting level");
		else if (elements_.end()[-2].name_!="file") throw std::domain_error("SnapshotXmlParser::startElement \"file\" not parent of \"event\"");

		tmp_.snapshotEvents_.push_back(
			SnapshotEvent
			(
				elements_.end()[-1].attributes_.at("backup-time"),
				elements_.end()[-1].attributes_.at("dump-level"),
				elements_.end()[-1].attributes_.at("action"),
				elements_.end()[-1].attributes_.at("start-volume-name"),
				elements_.end()[-1].attributes_.at("modification-time"),
				elements_.end()[-1].attributes_.at("size"),
				elements_.end()[-1].attributes_.at("sha3-512")
			)
		);
	}
	else throw std::runtime_error("SnapshotXmlParser::startElement: unrecognized event");
	std::cerr << std::string(elements_.size(),'\t') << "<" << name << ">" << std::endl;
}

void SnapshotXmlParser::endElement(const XML_Char *name){
	std::cerr << std::string(elements_.size(),'\t') << "</" << name << ">" << std::endl;

	if (elements_.back().name_=="snapshot"){
		if (elements_.size()!=1) throw std::domain_error("SnapshotXmlParser::endElement: \"snapshot\" wrong nesting level");
	}
	else if (elements_.back().name_=="file"){
		if (elements_.size()!=2) throw std::domain_error("SnapshotXmlParser::endElement: \"event\" wrong nesting level");
		else if (elements_.end()[-2].name_!="snapshot") throw std::domain_error("SnapshotXmlParser::endElement \"snapshot\" not parent of \"event\"");
		fileEntries_.push_back(tmp_);
	}
	else if (elements_.back().name_=="event"){
		if (elements_.size()!=3) throw std::domain_error("SnapshotXmlParser::endElement: \"event\" wrong nesting level");
		else if (elements_.end()[-2].name_!="file") throw std::domain_error("SnapshotXmlParser::endElement \"file\" not parent of \"event\"");
	}
	else throw std::runtime_error("SnapshotXmlParser::startElement: unrecognized event");
}

void SnapshotXmlParser::characterData(XML_Char const *s, int len){
	std::cerr << "SnapshotXmlParser::characterData: \"" << std::string(s,len) << "\"" << std::endl;
}
