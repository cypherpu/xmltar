/*

XmltarMemberCreate.hpp
Copyright 2017-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: Nov 26, 2017
Author: dbetz

This file is part of Xmltar.

Xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Xmltar.  If not, see <https://www.gnu.org/licenses/>.

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
