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

class Snapshot {

public:
	std::vector<SnapshotFileEntry> fileEntries_;

	Snapshot();
	Snapshot(std::string const & xmlFile);

	void Load(std::string const & xmlFile);
	void dump(std::ostream & os);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
