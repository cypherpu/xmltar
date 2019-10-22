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

#include "Snapshot/Snapshot.hpp"
#include "XmlParser/XmlParser.hpp"

class SnapshotXmlParser : public XmlParser {
	std::fstream fs_;
public:
	std::vector<std::shared_ptr<SnapshotEvent>> events_;
	std::deque<std::shared_ptr<SnapshotFileEntry>> fileEntries_;
public:
	SnapshotXmlParser(){
	}

	virtual void startElement(const XML_Char *name, const XML_Char **atts) override;
	virtual void endElement(const XML_Char *name) override;
	virtual void characterData(XML_Char const *s, int len) override;
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTXMLPARSER_HPP_ */
