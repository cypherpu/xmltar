/*
 * Ext4.hpp
 *
 *  Created on: Nov 4, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_METADATA_EXT4_HPP_
#define SRC_COMMON_METADATA_EXT4_HPP_

#include <boost/filesystem.hpp>

#include "Metadata/Metadata.hpp"
#include "Metadata/ExtendedAttribute.hpp"
#include "Metadata/FullTime.hpp"

class Ext4 : public Metadata {
	boost::filesystem::path path_;
	std::vector<ExtendedAttribute> extended_attributes;
	int mode;
	FullTime atime;
	FullTime ctime;
	FullTime mtime;
	int uid;
	std::string uname;
	int gid;
	std::string gname;
	size_t file_size;

public:
	Ext4(){}
	~Ext4(){}

	std::string toString(XmltarOptions & options);
};



#endif /* SRC_COMMON_METADATA_EXT4_HPP_ */
