/*
 * XmltarArchive.hpp
 *
 *  Created on: Nov 21, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_

#include <queue>

#include "Options/XmltarOptions.hpp"
#include "Utilities/PathCompare.hpp"

class PartialFileRead {
public:
	std::string filename_;
	std::ios::streampos nextPositionToBeRead_;
};

class XmltarArchive {
	XmltarOptions options_;
	std::string filename_;
	unsigned int volumeNumber_;
	std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> & filesToBeArchived_;
	std::streampos position_;
public:
	XmltarArchive(
		XmltarOptions & opts,
		std::string filename,
		unsigned int volumeNumber,
		std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> & filesToBeArchived,
		std::streampos position);

	PartialFileRead create(unsigned int volumeNumber);
	PartialFileRead append(unsigned int volumeNumber);

	std::string ArchiveHeader(std::string filename, int archive_sequence_number);
	std::string ArchiveTrailerBegin();
	std::string ArchiveTrailerMiddle(unsigned int padding);
	std::string ArchiveTrailerEnd();

	std::string CompressedArchiveHeader(std::string filename, int archive_sequence_number);
	std::string CompressedArchiveTrailer(unsigned int padding);

	bool static IsPaddingTrailer(std::string s);
	bool IsCompressedPaddingTrailer(std::fstream & iofs, std::ios::off_type offset);
	bool ranOutOfFiles(){
		return filesToBeArchived_.empty();
	}
	bool ranOutOfSpace(){

	}
};



#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_ */
