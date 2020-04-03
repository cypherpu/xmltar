/*
 * XmltarArchiveCreateSingleVolume.cpp
 *
 *  Created on: Feb 24, 2019
 *      Author: dbetz
 */

#include <fstream>

#include "Compressors/Compressor.hpp"
#include "Xmltar/XmltarArchiveCreateSingleVolume.hpp"
#include "Generated/Utilities/Debug2.hpp"

XmltarArchiveCreateSingleVolume::XmltarArchiveCreateSingleVolume(
		XmltarGlobals & globals,
		std::string filename,
		unsigned int volumeNumber
	)
	: XmltarArchive(globals,filename,volumeNumber)
{
	betz::Debug2 dbg("XmltarArchiveCreateSingleVolume::XmltarArchiveCreateSingleVolume");
	// std::shared_ptr<CompressorInterface> archiveCompression(globals_.options_.archiveCompression_.get()->clone());

	std::cerr << dbg << " starting archive **********" << std::endl;
	std::ostream *ofs;
	std::ofstream outputFileStream;
	if (filename_=="-")
		ofs=&std::cout;
	else {
		outputFileStream.open(filename);
		ofs=&outputFileStream;
	}
	globals_.current_xmltar_file_name_=std::filesystem::path(filename).filename().string();
	*ofs << globals_.options_.archiveCompression_->Open();
	*ofs << CompressedArchiveHeader(filename_,volumeNumber);
	std::cerr << dbg << ": " << globals_.filesToBeIncluded_.size() << std::endl;

	globals_.NextMember();

	for( ; globals_.nextMember_; globals_.NextMember()){
		std::cerr << dbg << ": " << globals_.nextMember_->filepath() << std::endl;

		if (globals_.nextMember_->isDirectory()){
			std::string tmp=globals_.nextMember_->MemberHeader()+globals_.nextMember_->MemberTrailer();
			*ofs << globals_.options_.archiveCompression_->ForceWrite(tmp);
			std::cerr << dbg << ": dir: bytes written=" << tmp.size() << " " << std::endl;
		}
		else if (globals_.nextMember_->isSymLink()){
			std::string tmp=globals_.nextMember_->MemberHeader()+globals_.nextMember_->MemberTrailer();
			*ofs << globals_.options_.archiveCompression_->ForceWrite(tmp);
			std::cerr << dbg << ": dir: bytes written=" << tmp.size() << " " <<  std::endl;
		}
		else if (globals_.nextMember_->isRegularFile()){
			std::cerr << "********** isRegularFile" << std::endl;
			std::cerr << dbg << ": archiving " << globals_.nextMember_->filepath().string() << std::endl;

			globals_.nextMember_->write(std::numeric_limits<size_t>::max(),*ofs);
		}

	}
	*ofs << globals_.options_.archiveCompression_->ForceWriteAndClose("");
	*ofs << CompressedArchiveTrailer();
}
