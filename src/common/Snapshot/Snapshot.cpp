/*
 * Snapshot.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#include <iostream>
#include <filesystem>

#include "Snapshot/Snapshot.hpp"
#include "Snapshot/SnapshotXmlParser.hpp"
#include "Options/XmltarOptions.hpp"
#include "Utilities/TemporaryFile.hpp"

Snapshot::Snapshot(XmltarOptions & options)
	: options_(options) {
	tempFile_=TemporaryFile(std::filesystem::temp_directory_path() / "xmltar_snapshot_XXXXXX");

	tempOfs_.open(tempFile_.string());

	tempOfs_
		<<	"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
			"<listed-incremental xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">\n";
}

Snapshot::~Snapshot(){
	tempOfs_
		<<	"</listed-incremental>";
	tempOfs_.close();

	std::filesystem::rename(tempFile_,options_.listed_incremental_file_.get());
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
#endif
