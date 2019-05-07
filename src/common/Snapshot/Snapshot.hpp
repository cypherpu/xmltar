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
	std::vector<std::filesystem::path> temporarySnapshotFilePaths_;
	std::filesystem::path temporarySnapshotDirPath_;
	std::ofstream temporarySnapshotFileOfs_;
	std::shared_ptr<Transform> temporaryFileCompression_;

	static std::string Prologue(){
		return  "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
				"<snapshot xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">\n";
	}

	static std::string Epilogue(){
		return "</snapshot>\n";
	}
public:
	Snapshot(XmltarOptions const & options, XmltarGlobals & globals);

	~Snapshot();

	void MergeSnapshotFiles();

	void NewTemporarySnapshotFile();
};

void MergeSnapshotFilesHelper(std::vector<std::filesystem::path> & sourcePaths, std::filesystem::path & targetPath, std::shared_ptr<Transform> compression);

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
