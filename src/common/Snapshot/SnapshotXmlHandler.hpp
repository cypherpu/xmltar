/*
 * SnapshotXmlHandler.hpp
 *
 *  Created on: Nov 20, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTXMLHANDLER_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTXMLHANDLER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <fstream>

class Snapshot;

#include "Snapshot/Snapshot.hpp"
#include "Xmltar/XmlHandler.hpp"
#include "Generated/Transform/Transform.hpp"

class SnapshotXmlHandler : public XmlHandler {
	std::unique_ptr<Transform> fileDecompression_;
	std::fstream fs_;
	Snapshot & snapshot_;
public:
	SnapshotXmlHandler(Snapshot & snapshot)
		: snapshot_(snapshot){
	}

	virtual void startElement(const XML_Char *name, const XML_Char **atts) override;
	virtual void endElement(const XML_Char *name) override;
	virtual void characterData(XML_Char const *s, int len) override;
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTXMLHANDLER_HPP_ */
