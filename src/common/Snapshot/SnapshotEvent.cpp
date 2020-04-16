/*

SnapshotEvent.cpp
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

#include <ostream>

#include "Snapshot/SnapshotEvent.hpp"

#if 0
std::ostream & operator<<(std::ostream &os, SnapshotEvent const & snapshotEvent){
	os << "<event "
			"backup-time=\"" << snapshotEvent.backupTime_ << "\" "
			"dump-level=\"" << snapshotEvent.dumpLevel_ << "\" "
			"action=\"" << snapshotEvent.action_ << "\" "
			"start-volume-name=\"" << snapshotEvent.startingVolumeName_ << "\" "
			"modification-time=\"" << snapshotEvent.modificationTime_ << "\" "
			"size=\"" << snapshotEvent.size_ << "\" "
			"sha3-512=\"" << snapshotEvent.sha3_512_ << "\" "
			<< "/>";

	return os;
}
#endif
