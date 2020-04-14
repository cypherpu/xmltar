/*

SnapshotFileEntry.cpp
Copyright 2019-2020 David A.Betz
David.Betz.MD@gmail.com
Created on: May 2, 2019
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

#include "Snapshot/SnapshotFileEntry.hpp"
#include "Utilities/XMLSafeString.hpp"

std::ostream & operator<<(std::ostream &os, SnapshotFileEntry const & snapshotFileEntry){
	os << "\t<file name=\"" << EncodeStringToXMLSafeString(snapshotFileEntry.pathname_.path()) << "\">\n";
	for(auto & i : snapshotFileEntry.snapshotEvents_){
		os << "\t\t" << i << std::endl;
	}
	os << "\t</file>" << std::endl;

	return os;
}
