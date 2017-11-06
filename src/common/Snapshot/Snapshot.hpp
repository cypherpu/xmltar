/*
 * Snapshot.hpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_

#include <vector>

#include "Snapshot/SnapshotOptions.hpp"
#include "Snapshot/SnapshotFileEntry.hpp"

class Snapshot {
public:
	SnapshotOptions options_;
	std::vector<SnapshotFileEntry> fileEntries_;

	Snapshot(){}
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
