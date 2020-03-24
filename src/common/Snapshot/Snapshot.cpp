/*
 * Snapshot.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#include <iostream>
#include <filesystem>

#include <boost/format.hpp>

#include "Xmltar/XmltarOptions.hpp"
#include "Xmltar/XmltarGlobals.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Snapshot/SnapshotXmlParser.hpp"
#include "Xmltar/XmltarOptions.hpp"
#include "Utilities/TemporaryFile.hpp"

Snapshot::Snapshot(XmltarGlobals & globals)
	: globals_(globals) {

	if (std::filesystem::exists(globals_.options_.listed_incremental_file_.get())){
		oldSnapshotFileIfs_.open(globals_.options_.listed_incremental_file_.get());
	}

	newSnapshotFileDirPath_=TemporaryDir(std::filesystem::temp_directory_path() / "xmltar_XXXXXX");
	newSnapshotFilePath_=newSnapshotFileDirPath_ / "new_snapshot_file.xml";
	newSnapshotFileOfs_.open(newSnapshotFilePath_.string());
}

Snapshot::~Snapshot(){
	newSnapshotFileOfs_.close();
	oldSnapshotFileIfs_.close();

	std::filesystem::copy(newSnapshotFilePath_,globals_.options_.listed_incremental_file_.get());
}

void Snapshot::NewTemporarySnapshotFile(){
	if (!temporarySnapshotFilePaths_.empty()){
		std::cerr << "Snapshot::NewTemporarySnapshotFile: closing " << temporarySnapshotFilePaths_.back() << std::endl;
		temporarySnapshotFileOfs_ << temporaryFileCompression_->ForceWriteAndClose(Epilogue());
		temporarySnapshotFileOfs_.close();
	}

    boost::format fmt("snapshot_%06d");
    fmt % temporarySnapshotFilePaths_.size();
	temporarySnapshotFilePaths_.push_back(temporarySnapshotDirPath_ / str(fmt));

	std::cerr << "Snapshot::NewTemporarySnapshotFile: opening " << temporarySnapshotFilePaths_.back() << std::endl;

	temporaryFileCompression_.reset(globals_.options_.incrementalFileCompression_->clone());
	temporaryFileCompression_->Open();

	temporarySnapshotFileOfs_.open(temporarySnapshotFilePaths_.back());

	temporarySnapshotFileOfs_ << temporaryFileCompression_->ForceWrite(Prologue());
}
