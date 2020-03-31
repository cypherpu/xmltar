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
#include "Snapshot/SnapshotXmlParser.hpp"
#include "Compressors/Compressor.hpp"

class XmltarOptions;
class XmltarGlobals;
class CompressorInterface;

class Snapshot {
public:
	XmltarGlobals & globals_;

	std::filesystem::path newSnapshotFileDirPath_;
	std::filesystem::path newSnapshotFilePath_;
	std::ofstream newSnapshotFileOfs_;
	std::shared_ptr<CompressorGeneralInterface> newSnapshotFileCompression_;

	std::ifstream oldSnapshotFileIfs_;

	std::deque<SnapshotFileEntry> fileEntries_;

	SnapshotXmlParser snapshotXmlParser_;

	bool finishedParsing_;

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

	void ReplenishFileEntries(){
		betz::Debug2("Snapshot::ReplenishFileEntries: ");
		if (fileEntries_.size()!=0 && fileEntries_.back().pathname_==ExtendedPath(ExtendedPath::PathType::MAX))
			return;

		while(fileEntries_.size()==0 && oldSnapshotFileIfs_){
			char buffer[1024];
			oldSnapshotFileIfs_.read(buffer,sizeof(buffer)/sizeof(char));
			snapshotXmlParser_.Parse(std::string(buffer,oldSnapshotFileIfs_.gcount()),false);
		}

		if (!oldSnapshotFileIfs_) fileEntries_.push_back(SnapshotFileEntry(ExtendedPath::PathType::MAX));
	}
	void CopyFrontFileEntryAndPop(){
		newSnapshotFileOfs_ << fileEntries_.front();
		fileEntries_.pop_front();
	}
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
