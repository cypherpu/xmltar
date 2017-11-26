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
#include "Options/XmltarOptions.hpp"

class Snapshot {

public:
	XmltarOptions options_;
	std::vector<SnapshotFileEntry> fileEntries_;

	Snapshot();
	Snapshot(std::string const & xmlFile);

	void Load(std::string const & xmlFile);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
