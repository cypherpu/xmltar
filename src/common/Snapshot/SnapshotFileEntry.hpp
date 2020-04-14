/*

SnapshotFileEntry.hpp
Copyright 2017-2020 David A. Betz
David.Betz.MD@gmail.com
Created on: Nov 4, 2017
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

	SnapshotFileEntry()
		: pathname_(), snapshotEvents_() {}

	SnapshotFileEntry(ExtendedPath const & pathname)
		: pathname_(pathname), snapshotEvents_() {}

	SnapshotFileEntry(ExtendedPath const & pathname, std::vector<SnapshotEvent> & snapshotEvents)
		: pathname_(pathname), snapshotEvents_(snapshotEvents) {}

	SnapshotFileEntry(SnapshotFileEntry const & snapshotFileEntry)
		: pathname_(snapshotFileEntry.pathname_), snapshotEvents_(snapshotFileEntry.snapshotEvents_) {}

	SnapshotEvent const & LastEvent(size_t dumpLevel){
		for(int i=snapshotEvents_.size()-1; i>=0; --i){
			if (snapshotEvents_[i].dumpLevel_<dumpLevel)
				return snapshotEvents_[i];
		}

		throw std::logic_error("SnapshotFileEntry::Last: no last Event");
	}

	friend std::ostream & operator<<(std::ostream &os, SnapshotFileEntry const & snapshotFileEntry);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRY_HPP_ */
