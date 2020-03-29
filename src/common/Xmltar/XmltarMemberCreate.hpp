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

#include "Utilities/Sha3.hpp"
#include "Xmltar/XmltarOptions.hpp"

class XmltarMemberCreate {
	XmltarGlobals & globals_;
public:
	std::filesystem::path const filepath_;
	struct stat stat_buf_;
	std::filesystem::file_status f_stat_;
	std::filesystem::file_type f_type_;
private:
	off_t file_size_;
	std::string memberHeader_;
	std::string memberTrailer_;
	std::shared_ptr<std::ifstream> ifs_;
public:
	std::string startingVolumeName_;
	Sha3_512 sha3sum512_;
private:
	bool metadataWritten_;

public:
	XmltarMemberCreate(XmltarGlobals & globals, std::filesystem::path const & filepath);
	~XmltarMemberCreate();

	void write(size_t numberOfFileBytesThatCanBeArchived, std::ostream & ofs);

	size_t MemberSize();
	std::string MemberHeader();
	std::string MemberTrailer();
	std::string CompressedMemberHeader();
	std::string CompressedMemberTrailer();
	size_t MinimumSize();
	size_t MaximumSize(size_t n);
	size_t NumberOfFileBytesThatCanBeArchived(size_t committedBytes, size_t pendingBytes);
	bool CanArchiveDirectory(size_t committedBytes, size_t pendingBytes);
	bool CanArchiveSymLink(size_t committedBytes, size_t pendingBytes);
	bool IsComplete(){
		std::cerr << "streamoff=" << ((std::streamoff)ifs_->tellg()) << "  size=" << file_size_ << std::endl;
		std::cerr << "!((bool)*ifs_)=" << !((bool)*ifs_) << std::endl;
		if (!*ifs_) return true;
		else return ifs_->tellg()==file_size_;
	}
	std::ifstream *Ifs(){ return ifs_.get(); }
	std::filesystem::path filepath(){ return filepath_; }
	bool isDirectory(){ return f_type_==std::filesystem::file_type::directory; }
	bool isSymLink(){ return f_type_==std::filesystem::file_type::symlink; }
	bool isRegularFile(){ return f_type_==std::filesystem::file_type::regular; }
	void RecalculateMemberHeader(){ memberHeader_=MemberHeader(); }
};



#endif /* SRC_COMMON_XMLTAR_XMLTARMEMBERCREATE_HPP_ */
