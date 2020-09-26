/*

Snapshot.cpp
Copyright 2017-2020 David A. Betz
David.Betz.MD@gmail.com
Created on: Nov 6, 2017
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
	: globals_(globals), snapshotXmlParser_(fileEntries_), finishedParsing_(false) {

	if (std::filesystem::exists(globals_.options_.listed_incremental_file_.value())){
		oldSnapshotFileIfs_.open(globals_.options_.listed_incremental_file_.value());
	}

	newSnapshotFileDirPath_=TemporaryDir(std::filesystem::temp_directory_path() / "xmltar_XXXXXX");
	newSnapshotFilePath_=newSnapshotFileDirPath_ / "new_snapshot_file.xml";
	newSnapshotFileOfs_.open(newSnapshotFilePath_.string());
	newSnapshotFileOfs_ << globals_.options_.snapshotEncryption_->Open(globals_.key_,"","");
	newSnapshotFileOfs_ << globals_.options_.snapshotEncryption_->Encrypt(
								globals_.options_.incrementalFileCompression_->Open()
							);

	newSnapshotFileOfs_ << globals_.options_.snapshotEncryption_->Encrypt(
								globals_.options_.incrementalFileCompression_->ForceWrite(Prologue())
							);

	if (oldSnapshotFileIfs_){
		std::string tmp;
		globals_.options_.incrementalFileDecompression_->Open();
		tmp+=globals_.options_.incrementalFileDecompression_->ForceWrite(
				globals_.options_.snapshotDecryption_->Open(
					globals_.key_
				)
			);
		snapshotXmlParser_.Parse(tmp,false);
	}
}

Snapshot::~Snapshot(){
	newSnapshotFileOfs_ << globals_.options_.snapshotEncryption_->Encrypt(
								globals_.options_.incrementalFileCompression_->ForceWriteAndClose(Epilogue())
							);
	newSnapshotFileOfs_ << globals_.options_.snapshotEncryption_->Close();

	newSnapshotFileOfs_.close();
	oldSnapshotFileIfs_.close();

	std::filesystem::copy(newSnapshotFilePath_,globals_.options_.listed_incremental_file_.value());
}

void Snapshot::ReplenishFileEntries(){
	betz::Debug2("Snapshot::ReplenishFileEntries: ");
	if (fileEntries_.size()!=0 && fileEntries_.back().pathname_==ExtendedPath(ExtendedPath::PathType::MAX))
		return;

	while(fileEntries_.size()==0 && oldSnapshotFileIfs_){
		char buffer[1024];
		oldSnapshotFileIfs_.read(buffer,sizeof(buffer)/sizeof(char));
		std::string tmp=globals_.options_.archiveDecompression_->ForceWrite(
				globals_.options_.archiveDecryption_->Decrypt(
					std::string(buffer,oldSnapshotFileIfs_.gcount()))
			);

		snapshotXmlParser_.Parse(tmp,false);
	}

	if (!oldSnapshotFileIfs_) fileEntries_.push_back(SnapshotFileEntry(ExtendedPath::PathType::MAX));
}

void Snapshot::CopyFrontFileEntryAndPop(){
	newSnapshotFileOfs_ << globals_.options_.snapshotEncryption_->Encrypt(
								globals_.options_.incrementalFileCompression_->ForceWrite(fileEntries_.front().ToString())
							);
	fileEntries_.pop_front();
}

