/*
 * SnapshotEvent.cpp
 *
 *  Created on: Mar 29, 2019
 *      Author: dbetz
 */

#include <ostream>

#include "Snapshot/SnapshotEvent.hpp"

std::ostream & operator<<(std::ostream &os, SnapshotEvent const & snapshotEvent){
	os << "<snapshot-event "
			"backup-time=\"" << snapshotEvent.backupTime_ << "\" "
			"volume-number=\"" << snapshotEvent.volumeNumber_ << "\" "
			"sha3-512=\"" << snapshotEvent.sha3_512_ << "\" "
			"dump-level=\"" << snapshotEvent.dumpLevel_ << "\""
			<< "/>";

	return os;
}
