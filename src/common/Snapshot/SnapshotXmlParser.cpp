/*
 * SnapshotXmlParser.cpp
 *
 *  Created on: Apr 21, 2019
 *      Author: dbetz
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
		fileEntries_.push_back(SnapshotFileEntry(filePath));
	}
	else if (elements_.back().name_=="event"){
		if (elements_.size()!=3) throw std::domain_error("SnapshotXmlParser::startElement: \"event\" wrong nesting level");
		else if (elements_.end()[-2].name_!="file") throw std::domain_error("SnapshotXmlParser::startElement \"file\" not parent of \"event\"");

		fileEntries_.end()[-1].snapshotEvents_.push_back(
			SnapshotEvent
			(
				elements_.end()[-1].attributes_.at("backup-time"),
				elements_.end()[-1].attributes_.at("dump-level"),
				elements_.end()[-1].attributes_.at("action"),
				elements_.end()[-1].attributes_.at("start-volume-number"),
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
