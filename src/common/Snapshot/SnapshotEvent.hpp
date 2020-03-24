/*
 * SnapshotEvent.hpp
 *
 *  Created on: Mar 29, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTEVENT_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTEVENT_HPP_

#include <string>

#include <boost/lexical_cast.hpp>

class SnapshotEvent {
public:
	time_t backupTime_;
	size_t dumpLevel_;
	std::string action_;
	std::string startingVolumeName_;
	time_t modificationTime_;
	size_t size_;
	std::string sha3_512_;

	SnapshotEvent(time_t backupTime, size_t dumpLevel, std::string action, std::string startingVolumeName, time_t modificationTime, size_t size, std::string sha3_512)
		: backupTime_(backupTime), dumpLevel_(dumpLevel), action_(action), startingVolumeName_(startingVolumeName), modificationTime_(modificationTime), size_(size), sha3_512_(sha3_512){}

	SnapshotEvent(std::string backupTimeString, std::string dumpLevelString, std::string action, std::string startingVolumeName, std::string modificationTime, std::string size, std::string sha3_512){
		backupTime_=boost::lexical_cast<time_t>(backupTimeString);
		dumpLevel_=boost::lexical_cast<size_t>(dumpLevelString);
		action_=action;
		startingVolumeName_=startingVolumeName;
		modificationTime_=boost::lexical_cast<time_t>(modificationTime);
		size_=boost::lexical_cast<size_t>(size);
		sha3_512_=sha3_512;
	}

	friend std::ostream & operator<<(std::ostream &os, SnapshotEvent const & snapshotEvent);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTEVENT_HPP_ */
