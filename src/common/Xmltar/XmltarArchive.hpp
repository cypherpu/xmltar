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
#include "Xmltar/XmltarMember.hpp"

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
	std::shared_ptr<XmltarMember> & nextMember_;
public:
	XmltarArchive(
		XmltarOptions & opts,
		std::string filename,
		unsigned int volumeNumber,
		std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> & filesToBeArchived,
		std::shared_ptr<XmltarMember> & nextMember
	);

	XmltarArchive(
		XmltarOptions & opts,
		std::string filename,
		std::shared_ptr<XmltarMember> & nextMember
	);

	PartialFileRead create(unsigned int volumeNumber);
	PartialFileRead append(unsigned int volumeNumber);

	std::string ArchiveHeader(std::string filename, int archive_sequence_number);
	std::string ArchiveTrailerBegin();
	std::string ArchiveTrailerMiddle(unsigned int padding);
	std::string ArchiveTrailerEnd();

	std::string CompressedArchiveHeader(std::string filename, int archive_sequence_number);
	std::string CompressedArchiveTrailer();
	std::string CompressedArchiveTrailer(unsigned int desiredLength);

	bool static IsPaddingTrailer(std::string s);
	bool IsCompressedPaddingTrailer(std::fstream & iofs, std::ios::off_type offset);
	bool ranOutOfFiles(){
		return filesToBeArchived_.empty();
	}
	bool ranOutOfSpace(){

	}

	std::shared_ptr<XmltarMember> NextMember(){
		if (filesToBeArchived_.empty()){
			return std::shared_ptr<XmltarMember>();
		}

		boost::filesystem::path const filepath=filesToBeArchived_.top();
		filesToBeArchived_.pop();
		boost::filesystem::file_status f_stat=boost::filesystem::symlink_status(filepath);

		if (boost::filesystem::is_directory(f_stat)){
			for(auto & p : boost::filesystem::directory_iterator(filepath) ){
				filesToBeArchived_.push(p);
			}
		}

		return std::make_shared<XmltarMember>(options_,filepath);
	}
};



#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_ */
