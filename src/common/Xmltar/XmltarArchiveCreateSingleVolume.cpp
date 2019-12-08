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
		XmltarOptions const & opts,
		XmltarGlobals & globals,
		std::string filename,
		unsigned int volumeNumber,
		std::unique_ptr<XmltarMemberCreate> & nextMember
	)
	: XmltarArchive(opts,globals,filename,volumeNumber,nextMember)
{
	betz::Debug2 dbg("XmltarArchiveCreateSingleVolume::XmltarArchiveCreateSingleVolume");
	std::shared_ptr<CompressorInterface> archiveCompression(options_.archiveCompression_.get()->clone());

	std::cerr << dbg << " starting archive **********" << std::endl;
	std::ostream *ofs;
	std::ofstream outputFileStream;
	if (filename_=="-")
		ofs=&std::cout;
	else {
		outputFileStream.open(filename);
		ofs=&outputFileStream;
	}
	archiveCompression->Open();
	*ofs << CompressedArchiveHeader(filename_,volumeNumber);
	std::cerr << dbg << ": " << globals_.filesToBeIncluded_.size() << std::endl;

	NextMember();

	for( ; nextMember_; NextMember()){
		std::cerr << dbg << ": " << nextMember_->filepath() << std::endl;

		if (nextMember_->isDirectory()){
			std::string tmp=nextMember_->MemberHeader()+nextMember_->MemberTrailer();
			std::string compressedDirectoryMember
				= options_.archiveMemberCompression_->OpenForceWriteAndClose(
						tmp
					);
			*ofs << archiveCompression->ForceWrite(compressedDirectoryMember);
			std::cerr << dbg << ": dir: bytes written=" << tmp.size() << " " << compressedDirectoryMember.size() << std::endl;
		}
		else if (nextMember_->isSymLink()){
			std::string tmp=nextMember_->MemberHeader()+nextMember_->MemberTrailer();
			std::string compressedDirectoryMember
				= options_.archiveMemberCompression_->OpenForceWriteAndClose(
						tmp
					);
			*ofs << archiveCompression->ForceWrite(compressedDirectoryMember);
			std::cerr << dbg << ": dir: bytes written=" << tmp.size() << " " << compressedDirectoryMember.size() << std::endl;
		}
		else if (nextMember_->isRegularFile()){
			std::cerr << "********** isRegularFile" << std::endl;
			std::cerr << dbg << ": archiving " << nextMember_->filepath().string() << std::endl;

			nextMember_->write(archiveCompression,std::numeric_limits<size_t>::max(),*ofs);
		}

	}
	*ofs << archiveCompression->ForceWriteAndClose("");
	*ofs << CompressedArchiveTrailer();
}
