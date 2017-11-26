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
	std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> & filesToBeArchived_;
public:
	XmltarArchive(
		XmltarOptions & opts,
		std::string filename,
		std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> & filesToBeArchived);

	PartialFileRead create(unsigned int volumeNumber);
	PartialFileRead append(unsigned int volumeNumber);

	std::string Header(std::string filename, int archive_sequence_number);
	std::string Trailer(unsigned int padding);

	bool static IsPaddingTrailer(std::string s);
	bool IsCompressedPaddingTrailer(std::fstream & iofs, std::ios::off_type offset);
	std::string Compress(std::string compressedContent);
	std::string Decompress(std::string compressedContent);

};



#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_ */
