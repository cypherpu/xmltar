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

class Snapshot;

#include "Snapshot/Snapshot.hpp"
#include "XmlParser/XmlParser.hpp"
#include "Transform/Transform.hpp"

class SnapshotXmlParser : public XmlParser {
	std::unique_ptr<Transform> fileDecompression_;
	std::fstream fs_;
	Snapshot & snapshot_;
public:
	SnapshotXmlParser(Snapshot & snapshot)
		: snapshot_(snapshot){
	}

	virtual void startElement(const XML_Char *name, const XML_Char **atts) override;
	virtual void endElement(const XML_Char *name) override;
	virtual void characterData(XML_Char const *s, int len) override;
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTXMLPARSER_HPP_ */
