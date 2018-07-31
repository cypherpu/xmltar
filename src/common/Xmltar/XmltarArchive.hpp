/*
 * XmltarArchive.hpp
 *
 *  Created on: Nov 21, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_

extern "C" {
#include <sys/stat.h>
}

#include <queue>

#include "Options/XmltarOptions.hpp"
#include "Utilities/PathCompare.hpp"
#include "Xmltar/XmltarMemberRegularFile.hpp"
#include "Xmltar/XmltarMemberBlock.hpp"
#include "Xmltar/XmltarMemberCharacter.hpp"
#include "Xmltar/XmltarMemberDirectory.hpp"
#include "Xmltar/XmltarMemberFifo.hpp"
#include "Xmltar/XmltarMemberSocket.hpp"
#include "Xmltar/XmltarMemberSymLink.hpp"

class PartialFileRead {
public:
	std::string filename_;
	std::ios::streampos nextPositionToBeRead_;
};

class XmltarArchive {
	XmltarOptions options_;
	std::string filename_;
	unsigned int volumeNumber_;
	std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> *filesToBeArchived_;
	std::shared_ptr<XmltarMember> & nextMember_;
public:
	XmltarArchive(
		XmltarOptions & opts,
		std::string filename,
		unsigned int volumeNumber,
		std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> *filesToBeArchived,
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
		return filesToBeArchived_->empty();
	}

	XmltarMember *xmltarMemberFactory(boost::filesystem::path const & filepath,boost::filesystem::file_status & f_stat, XmltarOptions & options){
		if (boost::filesystem::is_regular(filepath))
			return new XmltarMemberRegularFile(options,filepath);
		else if (boost::filesystem::is_directory(filepath))
			return new XmltarMemberDirectory(options,filepath);
		else if (boost::filesystem::is_symlink(filepath))
			return new XmltarMemberSymLink(options,filepath);

		struct stat statbuf;

		if (S_ISBLK(statbuf.st_mode))
			return new XmltarMemberSymLink(options,filepath);
		else if (S_ISCHR(statbuf.st_mode))
			return new XmltarMemberCharacter(options,filepath);
		else if (S_ISFIFO(statbuf.st_mode))
			return new XmltarMemberFifo(options,filepath);
		else if (S_ISSOCK(statbuf.st_mode))
			return new XmltarMemberSocket(options,filepath);
		else
			throw std::runtime_error("XmltarArchive::xmltarMemberFactory: unhandled file type");
	}

	std::shared_ptr<XmltarMember> NextMember(){
		if (filesToBeArchived_->empty()){
			return std::shared_ptr<XmltarMember>();
		}

		boost::filesystem::path const filepath=filesToBeArchived_->top();
		filesToBeArchived_->pop();
		boost::filesystem::file_status f_stat=boost::filesystem::symlink_status(filepath);

		if (boost::filesystem::is_directory(f_stat)){
			for(auto & p : boost::filesystem::directory_iterator(filepath) ){
				filesToBeArchived_->push(p);
			}
		}

		return std::shared_ptr<XmltarMember>(xmltarMemberFactory(filepath, f_stat, options_));
	}
};



#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_ */
