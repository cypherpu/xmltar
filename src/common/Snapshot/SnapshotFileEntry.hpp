/*
 * SnapshotFileEntry.hpp
 *
 *  Created on: Nov 4, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_

class SnapshotFileEntry {
public:
	std::string pathname_;
	time_t modificationTime_;
	std::string checksum_;
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_ */
