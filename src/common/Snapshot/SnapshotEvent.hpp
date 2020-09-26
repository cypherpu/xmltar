/*

SnapshotEvent.hpp
Copyright 2019-2020 David A. Betz
David.Betz.MD@gmail.com
Created on: Mar 29, 2019
Author: dbetz

This file is part of Xmltar.

Xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Xmltar.  If not, see <https://www.gnu.org/licenses/>.

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
	__off_t size_;
	std::string sha3_512_;

	SnapshotEvent(time_t backupTime, size_t dumpLevel, std::string action, std::string startingVolumeName, time_t modificationTime, size_t size, std::string sha3_512)
		: backupTime_(backupTime), dumpLevel_(dumpLevel), action_(action), startingVolumeName_(startingVolumeName), modificationTime_(modificationTime), size_(size), sha3_512_(sha3_512){}

	SnapshotEvent(std::string backupTimeString, std::string dumpLevelString, std::string action, std::string startingVolumeName, std::string modificationTime, std::string size, std::string sha3_512){
		backupTime_=boost::lexical_cast<time_t>(backupTimeString);
		dumpLevel_=boost::lexical_cast<size_t>(dumpLevelString);
		action_=action;
		startingVolumeName_=startingVolumeName;
		modificationTime_=boost::lexical_cast<time_t>(modificationTime);
		size_=boost::lexical_cast<__off_t>(size);
		sha3_512_=sha3_512;
	}

	std::string ToString(){
		std::ostringstream oss;

		oss << "<event "
				"backup-time=\"" << backupTime_ << "\" "
				"dump-level=\"" << dumpLevel_ << "\" "
				"action=\"" << action_ << "\" "
				"start-volume-name=\"" << startingVolumeName_ << "\" "
				"modification-time=\"" << modificationTime_ << "\" "
				"size=\"" << size_ << "\" "
				"sha3-512=\"" << sha3_512_ << "\" "
				<< "/>";

		return oss.str();

	}
	// friend std::ostream & operator<<(std::ostream &os, SnapshotEvent const & snapshotEvent);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTEVENT_HPP_ */
