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
protected:
	XmltarOptions const  & options_;
	boost::filesystem::path const filepath_;
	struct stat stat_buf;
	boost::filesystem::file_status f_stat;
	boost::filesystem::file_type f_type;
	off_t file_size;
	std::string memberHeader_;
	std::string memberTrailer_;
	size_t nextByte_;
	bool isArchived_;

public:
	XmltarMember(XmltarOptions const & options, boost::filesystem::path const & filepath);
	virtual ~XmltarMember(){}

	bool completed(){ return isArchived_; }
	virtual void write(std::shared_ptr<Transform> archiveCompression, size_t committedBytes, size_t pendingBytes, std::ostream & ofs);

	virtual std::string MemberHeader();
	virtual std::string MemberTrailer();
	virtual std::string CompressedMemberHeader();
	virtual std::string CompressedMemberTrailer();
	virtual size_t NumberOfFileBytesThatCanBeArchived(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression);
	virtual bool CanArchive(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression)=0;
	virtual bool IsComplete();
	virtual boost::filesystem::path filepath();
	virtual bool isDirectory();
	virtual bool isSymLink();
	virtual bool isRegularFile();
	virtual size_t NextByte();
	virtual void RecalculateMemberHeader();
};



#endif /* SRC_COMMON_XMLTAR_XMLTARMEMBER_HPP_ */
