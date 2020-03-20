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
#include "Compressors/Compressor.hpp"

class XmltarOptions;
class XmltarGlobals;
class CompressorInterface;

class Snapshot {
public:
	XmltarGlobals & globals_;
	std::filesystem::path newSnapshotFileDirPath_;
	std::vector<std::filesystem::path> temporarySnapshotFilePaths_;

	std::ofstream temporarySnapshotFileOfs_;
	std::shared_ptr<CompressorGeneralInterface> temporaryFileCompression_;

	static std::string Prologue(){
		return  "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
				"<snapshot xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">\n";
	}

	static std::string Epilogue(){
		return "</snapshot>\n";
	}
public:
	Snapshot(XmltarGlobals & globals);

	~Snapshot();

	void MergeSnapshotFiles();
	void MergeSnapshotFilesHelper(std::vector<std::filesystem::path> & sourcePaths, std::filesystem::path & targetPath, std::shared_ptr<CompressorGeneralInterface> compression);
	void NewTemporarySnapshotFile();
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
