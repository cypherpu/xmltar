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
	boost::filesystem::path const & filepath_;
	struct stat stat_buf;
	boost::filesystem::file_status f_stat;
	boost::filesystem::file_type f_type;
	off_t file_size;
	std::string memberHeader_;
	std::string memberTrailer_;
	std::unique_ptr<Transform> precompression_;
	std::unique_ptr<Transform> encoding_;
	std::unique_ptr<Transform> memberCompression_;
	size_t nextByte_;

public:
	XmltarMember(XmltarOptions const & options, boost::filesystem::path const & filepath);

	bool completed(){ return nextByte_>=file_size; }
	std::tuple<size_t,std::shared_ptr<Transform> > write(size_t remainingArchiveSize, std::shared_ptr<Transform> archiveCompression, std::ostream & ofs);

	size_t MemberSize();
	std::string MemberHeader();
	std::string MemberTrailer();
	std::string CompressedMemberHeader();
	std::string CompressedMemberTrailer();
	size_t MinimumSize();
	size_t MaximumSize(size_t n);
};



#endif /* SRC_COMMON_XMLTAR_XMLTARMEMBER_HPP_ */
