/*
 * SnapshotXmlParser.hpp
 *
 *  Created on: Nov 20, 2017
 *      Author: dbetz
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
