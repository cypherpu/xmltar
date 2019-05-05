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
#include <memory>
#include <filesystem>

#include "Snapshot/SnapshotFileEntry.hpp"

class XmltarOptions;
class XmltarGlobals;
class Transform;

class Snapshot {
public:
	XmltarOptions const & options_;
	XmltarGlobals & globals_;
	std::vector<std::filesystem::path> incrementalSnapshotFilePaths_;
	std::filesystem::path temporarySnapshotFilePath_;
	std::ofstream temporarySnapshotFileOfs_;
	std::shared_ptr<Transform> temporaryFileCompression_;
public:
	Snapshot(XmltarOptions const & options, XmltarGlobals & globals);

	~Snapshot();

	void MergeSnapshotFiles();
};

void MergeSnapshotFilesHelper(std::vector<std::filesystem::path> & sourcePaths, std::filesystem::path & targetPath, std::shared_ptr<Transform> compression);

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
