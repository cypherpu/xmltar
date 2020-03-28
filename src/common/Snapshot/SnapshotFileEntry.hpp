/*
 * SnapshotFileEntry.hpp
 *
 *  Created on: Nov 4, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_

#include <vector>
#include <string>
#include <filesystem>

#include "Utilities/ExtendedPath.hpp"
#include "Snapshot/SnapshotEvent.hpp"

class SnapshotFileEntry {
public:
	ExtendedPath pathname_;
	std::vector<SnapshotEvent> snapshotEvents_;

	SnapshotFileEntry(ExtendedPath const & pathname)
		: pathname_(pathname), snapshotEvents_() {}

	SnapshotFileEntry(ExtendedPath const & pathname, std::vector<SnapshotEvent> & snapshotEvents)
		: pathname_(pathname), snapshotEvents_(snapshotEvents) {}

	std::string LastAction(size_t dumpLevel){
		for(int i=snapshotEvents_.size()-1; i>=0; --i){
			if (snapshotEvents_[i].dumpLevel_<dumpLevel)
				return snapshotEvents_[i].action_;
		}

		return "";
	}

	friend std::ostream & operator<<(std::ostream &os, SnapshotFileEntry const & snapshotFileEntry);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_ */
