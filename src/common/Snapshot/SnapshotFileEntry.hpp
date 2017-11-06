/*
 * SnapshotFileEntry.hpp
 *
 *  Created on: Nov 4, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_

#include <boost/filesystem.hpp>
#include <vector>
#include <string>

class SnapshotFileEntry {
public:
	boost::filesystem::path pathname_;
	time_t lastBackupEpochTime_;
	std::string lastBackupAsciiTime_;
	std::vector<int> mediaNumbers_;
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_ */
