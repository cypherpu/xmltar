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
	struct XAttrs {
		std::string name_;
		std::string value_;
	};
	std::vector<XAttrs> xAttrs_;

protected:
	XmltarOptions const  & options_;
	boost::filesystem::path const filepath_;
	struct stat stat_buf_;
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

	void populateXAttrs();
	bool completed(){ return isArchived_; }
	virtual void write(std::shared_ptr<Transform> archiveCompression, size_t committedBytes, size_t pendingBytes, std::ostream & ofs)=0;

	std::string commonHeader();
	virtual std::string MemberHeader()=0;
	virtual std::string MemberTrailer()=0;
	std::string CompressedMemberHeader(){ return options_.archiveMemberCompression_.get()->CompressString(MemberHeader()); }
	std::string CompressedMemberTrailer(){ return options_.archiveMemberCompression_.get()->CompressString(MemberTrailer()); }
	virtual bool CanArchive(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression)=0;
	std::string filepath(){ return filepath_.string(); }
	size_t NextByte(){ return nextByte_; }
	virtual void RecalculateMemberHeader(){	memberHeader_=MemberHeader(); }
};



#endif /* SRC_COMMON_XMLTAR_XMLTARMEMBER_HPP_ */
