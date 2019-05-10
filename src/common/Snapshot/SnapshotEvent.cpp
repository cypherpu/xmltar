/*
 * SnapshotEvent.cpp
 *
 *  Created on: Mar 29, 2019
 *      Author: dbetz
 */

#include <ostream>

#include "Snapshot/SnapshotEvent.hpp"

std::ostream & operator<<(std::ostream &os, SnapshotEvent const & snapshotEvent){
	os << "<event "
			"backup-time=\"" << snapshotEvent.backupTime_ << "\" "
			"dump-level=\"" << snapshotEvent.dumpLevel_ << "\" "
			"volume-number=\"" << snapshotEvent.volumeNumber_ << "\" "
			"sha3-512=\"" << snapshotEvent.sha3_512_ << "\" "
			<< "/>";

	return os;
}
