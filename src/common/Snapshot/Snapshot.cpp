/*
 * Snapshot.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#include <iostream>

#include <boost/format.hpp>

#include "Xmltar/XmltarOptions.hpp"
#include "Xmltar/XmltarGlobals.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Snapshot/SnapshotXmlParser.hpp"
#include "Xmltar/XmltarOptions.hpp"
#include "Utilities/TemporaryFile.hpp"

class IncrementalFile {
public:
	std::ifstream incrementalSnapshotIfs_;
	std::unique_ptr<Transform> incrementalSnapshotDecompression_;
	SnapshotXmlParser incrementalSnapshotParser_;

	IncrementalFile(std::filesystem::path & filename, std::shared_ptr<Transform> decompression)
		: incrementalSnapshotIfs_(filename), incrementalSnapshotDecompression_(decompression->clone()) {
	}
};

void MergeSnapshotFilesHelper(std::vector<std::filesystem::path> & sourcePaths, std::filesystem::path & targetPath, std::shared_ptr<Transform> compression){
	std::vector<std::shared_ptr<IncrementalFile>> incrementalFiles;

	for(auto & i : sourcePaths){
		if (std::filesystem::exists(i) && std::filesystem::file_size(i)!=0){
			incrementalFiles.push_back(std::make_shared<IncrementalFile>(i,compression));
			if (!incrementalFiles.back()->incrementalSnapshotIfs_)
				incrementalFiles.erase(incrementalFiles.end()-1);
			incrementalFiles.back()->incrementalSnapshotDecompression_->OpenDecompression();
		}
	}

	std::ofstream ofs(targetPath);
	std::shared_ptr<Transform> targetCompression(compression->clone());

	ofs << targetCompression->ForceWrite(Snapshot::Prologue());

	for( ; ; ){
		for(size_t i=0; i<incrementalFiles.size(); ){
			std::cerr << "i=" << i << std::endl;
			if (incrementalFiles[i]->incrementalSnapshotIfs_){
				while(incrementalFiles[i]->incrementalSnapshotParser_.fileEntries_.size()==0){
					XML_Char buffer[1024];
					incrementalFiles[i]->incrementalSnapshotIfs_.read(buffer,sizeof(buffer)/sizeof(*buffer));
					std::string tmp=incrementalFiles[i]->incrementalSnapshotDecompression_->ForceWrite(std::string(buffer,incrementalFiles[i]->incrementalSnapshotIfs_.gcount()));
					incrementalFiles[i]->incrementalSnapshotParser_.Parse(tmp,false);
					if (!incrementalFiles[i]->incrementalSnapshotIfs_){
						incrementalFiles[i]->incrementalSnapshotIfs_.close();
						std::string tmp=incrementalFiles[i]->incrementalSnapshotDecompression_->ForceWriteAndClose("");
						incrementalFiles[i]->incrementalSnapshotParser_.Parse(tmp,true);
						break;
					}
				}
			}

			if (incrementalFiles[i]->incrementalSnapshotParser_.fileEntries_.size()==0){
				incrementalFiles.erase(incrementalFiles.begin()+i);
				continue;
			}
			else ++i;
		}

		if (incrementalFiles.size()==0) break;

		std::filesystem::path smallestPathname=incrementalFiles[0]->incrementalSnapshotParser_.fileEntries_.front()->pathname_;

		for(size_t i=1; i<incrementalFiles.size(); ++i)
			if (incrementalFiles[i]->incrementalSnapshotParser_.fileEntries_.front()->pathname_<smallestPathname)
				smallestPathname=incrementalFiles[i]->incrementalSnapshotParser_.fileEntries_.front()->pathname_;

		std::cerr << "smallestPathname=" << smallestPathname << std::endl;

		SnapshotFileEntry snapshotFileEntry(smallestPathname);

		for(auto & i : incrementalFiles)
			if (i->incrementalSnapshotParser_.fileEntries_.front()->pathname_==smallestPathname){
				for(auto & j : i->incrementalSnapshotParser_.fileEntries_.front()->snapshotEvents_)
					snapshotFileEntry.snapshotEvents_.push_back(j);

				i->incrementalSnapshotParser_.fileEntries_.erase(i->incrementalSnapshotParser_.fileEntries_.begin());
			}

		std::ostringstream oss;
		oss << snapshotFileEntry;
		ofs << targetCompression->ForceWrite(oss.str());
	}

	targetCompression->ForceWriteAndClose("</snapshot>");
}

void Snapshot::MergeSnapshotFiles(){
}

Snapshot::Snapshot(XmltarOptions const & options, XmltarGlobals & globals)
	: options_(options), globals_(globals) {

	temporarySnapshotDirPath_=TemporaryDir(std::filesystem::temp_directory_path() / "xmltar_XXXXXX");
}

Snapshot::~Snapshot(){
	temporarySnapshotFileOfs_ << temporaryFileCompression_->ForceWriteAndClose(Epilogue());
	temporarySnapshotFileOfs_.close();

#if 0
	std::filesystem::rename(newSnapshotFilePath,options_.listed_incremental_file_.get());
#endif
}

void Snapshot::NewTemporarySnapshotFile(){
	if (temporarySnapshotFilePaths_.size()){
		temporarySnapshotFileOfs_ << temporaryFileCompression_->ForceWriteAndClose(Epilogue());
		temporarySnapshotFileOfs_.close();
	}

    boost::format fmt("snapshot_%06d");
    fmt % temporarySnapshotFilePaths_.size();
	temporarySnapshotFilePaths_.push_back(temporarySnapshotDirPath_ / str(fmt));

	temporaryFileCompression_.reset(options_.incrementalFileCompression_->clone());
	temporaryFileCompression_->OpenCompression();

	temporarySnapshotFileOfs_.open(temporarySnapshotFilePaths_.back());

	temporarySnapshotFileOfs_ << temporaryFileCompression_->ForceWrite(Prologue());
}
