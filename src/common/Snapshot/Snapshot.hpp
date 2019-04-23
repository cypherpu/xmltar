/*
 * Snapshot.hpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_

#include <vector>
#include <string>

#include "Snapshot/SnapshotFileEntry.hpp"

class XmltarOptions;

class Snapshot {
	XmltarOptions & options_;
public:
	std::vector<SnapshotFileEntry> fileEntries_;

	Snapshot(XmltarOptions & options);
	Snapshot(XmltarOptions & options, std::string const & xmlFile);

	void load(std::string const & xmlFile);
	void dump(std::ostream & os);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
