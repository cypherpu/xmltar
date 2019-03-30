/*
 * SnapshotEvent.hpp
 *
 *  Created on: Mar 29, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTEVENT_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTEVENT_HPP_

#include <string>

class SnapshotEvent {
public:
	time_t backupTime_;
	size_t volumeNumber_;
	std::string sha3_512_;
	size_t dumpLevel_;

	friend std::ostream & operator<<(std::ostream &os, SnapshotEvent const & snapshotEvent);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTEVENT_HPP_ */
