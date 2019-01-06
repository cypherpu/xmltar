/*
 * XmltarMember.hpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARMEMBER_HPP_
#define SRC_COMMON_XMLTAR_XMLTARMEMBER_HPP_

#include "Options/XmltarOptions.hpp"
#include <Transform/Transform.hpp>

extern "C" {
#include <sys/stat.h>
}

class XmltarMember {
	XmltarOptions const  & options_;
	boost::filesystem::path const filepath_;
	struct stat stat_buf;
	boost::filesystem::file_status f_stat;
	boost::filesystem::file_type f_type;
	off_t file_size;
	std::string memberHeader_;
	std::string memberTrailer_;
	size_t nextByte_;

public:
	XmltarMember(XmltarOptions const & options, boost::filesystem::path const & filepath);

	bool completed(){
		// nextByte_ (size_t) is unsigned
		// file_size (off_t) is signed
		if (file_size<0)
			throw std::logic_error("");
		else
			return nextByte_>=(size_t) file_size;
	}
	void write(std::shared_ptr<Transform> archiveCompression, size_t numberOfFileBytesThatCanBeArchived, std::ostream & ofs);

	size_t MemberSize();
	std::string MemberHeader();
	std::string MemberTrailer();
	std::string CompressedMemberHeader();
	std::string CompressedMemberTrailer();
	size_t MinimumSize();
	size_t MaximumSize(size_t n);
	size_t NumberOfFileBytesThatCanBeArchived(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression);
	bool CanArchiveDirectory(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression);
	bool CanArchiveSymLink(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression);
	bool IsComplete(){
		// nextByte_ (size_t) is unsigned
		// file_size (off_t) is signed
		if (file_size<0)
			throw std::logic_error("");
		else
			return nextByte_==(size_t) file_size;
	}
	boost::filesystem::path filepath(){ return filepath_; }
	bool isDirectory(){ return f_type==boost::filesystem::file_type::directory_file; }
	bool isSymLink(){ return f_type==boost::filesystem::file_type::symlink_file; }
	bool isRegularFile(){ return f_type==boost::filesystem::file_type::regular_file; }
	size_t NextByte(){ return nextByte_; }
	void RecalculateMemberHeader(){ memberHeader_=MemberHeader(); }
};



#endif /* SRC_COMMON_XMLTAR_XMLTARMEMBER_HPP_ */
