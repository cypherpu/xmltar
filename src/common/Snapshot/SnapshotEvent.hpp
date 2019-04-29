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
	size_t volumeNumber_;
	std::string sha3_512_;

	SnapshotEvent(time_t backupTime, size_t dumpLevel, size_t volumeNumber, std::string sha3_512)
		: backupTime_(backupTime), dumpLevel_(dumpLevel), volumeNumber_(volumeNumber), sha3_512_(sha3_512){}

	SnapshotEvent(std::string backupTimeString, std::string dumpLevelString, std::string volumeNumberString, std::string sha3_512){
		backupTime_=boost::lexical_cast<time_t>(backupTimeString);
		dumpLevel_=boost::lexical_cast<size_t>(dumpLevelString);
		volumeNumber_=boost::lexical_cast<size_t>(volumeNumberString);
		sha3_512_=sha3_512;
	}

	friend std::ostream & operator<<(std::ostream &os, SnapshotEvent const & snapshotEvent);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTEVENT_HPP_ */
