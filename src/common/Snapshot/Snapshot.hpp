/*
 * Snapshot.hpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_

#include <vector>
#include <string>
#include <fstream>

#include "Snapshot/SnapshotFileEntry.hpp"

class XmltarOptions;

class Snapshot {
	XmltarOptions & options_;
	std::filesystem::path tempFile_;
	std::ofstream tempOfs_;
public:
	Snapshot(XmltarOptions & options);

	~Snapshot();
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
