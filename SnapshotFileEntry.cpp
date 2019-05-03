/*
 * SnapshotFileEntry.cpp
 *
 *  Created on: May 2, 2019
 *      Author: dbetz
 */

#include "Snapshot/SnapshotFileEntry.hpp"

std::ostream & operator<<(std::ostream &os, SnapshotFileEntry const & snapshotFileEntry){
	os << "\t<file name=\"" << EncodeStringToXMLSafeString(snapshotFileEntry.pathname_) << "\">\n";
	for(auto & i : snapshotFileEntry.snapshotEvents_){
		os << "\t\t" << i << std::end;
	}
	os << "\t</file>" << std::endl;
}
