/*
 * Snapshot.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#include <iostream>

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

	ofs << targetCompression->ForceWrite(
			"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
			"<snapshot xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">\n"
	);

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

	temporaryFileCompression_.reset(options_.incrementalFileCompression_->clone());
	temporaryFileCompression_->OpenCompression();

	temporarySnapshotFilePath_=TemporaryFile(std::filesystem::temp_directory_path() / "xmltar_incremental_snapshot_XXXXXX");

	temporarySnapshotFileOfs_.open(temporarySnapshotFilePath_.string());

	temporarySnapshotFileOfs_ << temporaryFileCompression_->ForceWrite(
			"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
			"<snapshot xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">\n"
	);
}

Snapshot::~Snapshot(){
	temporarySnapshotFileOfs_ << temporaryFileCompression_->ForceWriteAndClose("</snapshot>");
	temporarySnapshotFileOfs_.close();

#if 0
	/* open and merge permanent listed incremental file and temporary listed incremental file */

	std::filesystem::path newSnapshotFilePath=TemporaryFile(std::filesystem::temp_directory_path() / "xmltar_new_snapshot_XXXXXX");

	std::ifstream temporarySnapshotFilePath_(tempoararySnapshotFilePath_.string());
	std::ifstream snapshotFileIfs(options_.listed_incremental_file_.get());
	std::ofstream newSnapshoFileOfs(newSnapshotFilePath.string());

	SnapshotXmlParser incrementalSnapshotParser;
	SnapshotXmlParser snapshotFileParser;

	std::shared_ptr<Transform> incrementalSnapshotDecompression(options_.incrementalFileCompression_->clone());
	std::shared_ptr<Transform> snapshotFileDecompression(options_.incrementalFileCompression_->clone());

	incrementalSnapshotDecompression->OpenDecompression();
	snapshotFileDecompression->OpenDecompression();

	XML_Char buffer[1024];

	std::string tmp;
	for( ; ; ){
		if (incrementalSnapshotParser.fileEntries_.size()==0 && incrementalSnapshotFileIfs_){
			incrementalSnapshotFileIfs_.read(buffer,sizeof(buffer)/sizeof(*buffer));
			tmp=incrementalSnapshotDecompression->ForceWrite(std::string(buffer,incrementalSnapshotFileIfs_.gcount()));
			incrementalSnapshotParser.Parse(tmp,false);
		}
		if (snapshotFileParser.fileEntries_.size()==0 && snapshotFileIfs){
			snapshotFileIfs.read(buffer,sizeof(buffer)/sizeof(*buffer));
			tmp=snapshotFileDecompression->ForceWrite(std::string(buffer,snapshotFileIfs.gcount()));
			snapshotFileParser.Parse(tmp,false);
		}
	}
	std::string tmp;
	while(ifs){
		ifs.read(buffer,sizeof(buffer)/sizeof(*buffer));

		tmp=memberDecompression->ForceWrite(archiveDecompression->ForceWrite(std::string(buffer,ifs.gcount())));
		//std::cerr << "ifs.gcount()=" << ifs.gcount() << std::endl;
		xmltarSingleVolumeHandler.Parse(tmp,false);
	}

	tmp=memberDecompression->ForceWriteAndClose(archiveDecompression->ForceWriteAndClose(""));
	//std::cerr << "ifs.gcount()=" << ifs.gcount() << std::endl;
	xmltarSingleVolumeHandler.Parse(tmp,false);
#endif

#if 0
	std::filesystem::rename(newSnapshotFilePath,options_.listed_incremental_file_.get());
#endif
}

#if 0
void Snapshot::load(std::string const & xmlFilename){
	std::ifstream ifs(xmlFilename);

	XML_Char buffer[1024];

	std::shared_ptr<Transform> archiveDecompression(options_.archiveCompression_->clone());

	archiveDecompression->OpenDecompression();

	SnapshotXmlParser snapshotXmlParser(*this);
	std::string tmp;
	while(ifs){
		ifs.read(buffer,sizeof(buffer)/sizeof(*buffer));

		tmp=archiveDecompression->ForceWrite(std::string(buffer,ifs.gcount()));
		//std::cerr << "ifs.gcount()=" << ifs.gcount() << std::endl;
		snapshotXmlParser.Parse(tmp,false);
	}

	tmp=archiveDecompression->ForceWriteAndClose("");
	//std::cerr << "ifs.gcount()=" << ifs.gcount() << std::endl;
	snapshotXmlParser.Parse(tmp,false);
}

void Snapshot::dump(std::ostream & os){
	for(auto & i : fileEntries_){
		os << "\t<file name=\"" << i.pathname_.string() << "\">\n";
		for(auto & j : i.snapshotEvents_){
			os << "\t\t" << j << "\n";
		}
	}
}

void Snapshot::NewTemporarySnapshotFile(){

}
#endif
