/*
 * XmltarArchiveCreateSingleVolume.cpp
 *
 *  Created on: Feb 24, 2019
 *      Author: dbetz
 */

#include <fstream>

#include "Xmltar/XmltarArchiveCreateSingleVolume.hpp"
#include "Debug2/Debug2.hpp"

XmltarArchiveCreateSingleVolume::XmltarArchiveCreateSingleVolume(
		XmltarOptions & opts,
		std::string filename,
		unsigned int volumeNumber,
		std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,PathCompare> *filesToBeArchived,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	)
	: XmltarArchive(opts,filename,volumeNumber,nextMember), filesToBeArchived_(filesToBeArchived)
{
	betz::Debug2 dbg("XmltarArchiveCreateSingleVolume::XmltarArchiveCreateSingleVolume");
	std::shared_ptr<Transform> archiveCompression(options_.archiveCompression_.get()->clone());

	std::cerr << dbg << " starting archive **********" << std::endl;
	std::ostream *ofs;
	std::ofstream outputFileStream;
	if (filename_=="-")
		ofs=&std::cout;
	else {
		outputFileStream.open(filename);
		ofs=&outputFileStream;
	}
	archiveCompression->OpenCompression();
	*ofs << CompressedArchiveHeader(filename_,volumeNumber);
	std::cerr << dbg << "XmltarArchive::XmltarArchive: " << filesToBeArchived->size() << std::endl;
	std::shared_ptr<XmltarMemberCreate> xmltarMember;
	for( ; !filesToBeArchived->empty(); ){
		std::cerr << dbg << "XmltarArchive::XmltarArchive: " << filesToBeArchived->top() << std::endl;

		std::filesystem::path const filepath=filesToBeArchived_->top();
		filesToBeArchived->pop();
		std::filesystem::file_status f_stat=std::filesystem::symlink_status(filepath);

		if (std::filesystem::is_directory(f_stat)){
			for(auto & p : std::filesystem::directory_iterator(filepath) ){
				filesToBeArchived->push(p);
			}
		}

		xmltarMember=std::make_shared<XmltarMemberCreate>(options_,filepath);
		xmltarMember->write(archiveCompression, std::numeric_limits<size_t>::max(), *ofs);
	}
	*ofs << archiveCompression->ForceWriteAndClose("");
	*ofs << CompressedArchiveTrailer();
}



