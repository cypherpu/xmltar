/*
 * XmltarMemberCreate.hpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARMEMBERCREATE_HPP_
#define SRC_COMMON_XMLTAR_XMLTARMEMBERCREATE_HPP_

#include <filesystem>
#include <fstream>

extern "C" {
#include <sys/stat.h>
}

#include "Xmltar/XmltarOptions.hpp"
#include "Transform/Transform.hpp"
#include "Transform/ProcessSha3sum512.hpp"

class XmltarMemberCreate {
	XmltarOptions const  & options_;
	XmltarGlobals & globals_;
	std::filesystem::path const filepath_;
	struct stat stat_buf;
	std::filesystem::file_status f_stat;
	std::filesystem::file_type f_type;
	off_t file_size;
	std::string memberHeader_;
	std::string memberTrailer_;
	std::shared_ptr<std::ifstream> ifs_;
	size_t startingVolume_;

	ProcessSha3sum512 sha3sum512_;

	bool metadataWritten_;

public:
	XmltarMemberCreate(XmltarOptions const & options, XmltarGlobals & globals, std::filesystem::path const & filepath);
	~XmltarMemberCreate();

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
		std::cerr << "streamoff=" << ((std::streamoff)ifs_->tellg()) << "  size=" << file_size << std::endl;
		std::cerr << "!((bool)*ifs_)=" << !((bool)*ifs_) << std::endl;
		return ifs_->tellg()==file_size;
	}
	std::ifstream & Ifs(){ return *ifs_; }
	std::filesystem::path filepath(){ return filepath_; }
	bool isDirectory(){ return f_type==std::filesystem::file_type::directory; }
	bool isSymLink(){ return f_type==std::filesystem::file_type::symlink; }
	bool isRegularFile(){ return f_type==std::filesystem::file_type::regular; }
	void RecalculateMemberHeader(){ memberHeader_=MemberHeader(); }
};



#endif /* SRC_COMMON_XMLTAR_XMLTARMEMBERCREATE_HPP_ */
