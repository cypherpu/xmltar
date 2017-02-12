/*

Copyright 2010 by David A. Betz

This file is part of xmltar.

xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xmltar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef Meta_Data_Ext4_hpp_
#define Meta_Data_Ext4_hpp_

#include <boost/fusion/include/adapt_struct.hpp>

////////////////////////////////////////////////
struct full_time {
    time_t posix_time_;
    std::string formatted_time_;
};
#if 1
BOOST_FUSION_ADAPT_STRUCT(
        full_time,
        (time_t, posix_time_)
        (std::string, formatted_time_)
)
#endif
////////////////////////////////////////////////
struct extended_attribute {
	std::string key_;
	std::string value_;

	// friend std::ostream& operator<<(std::ostream&, const gnc_space_and_id&);
};
#if 1
BOOST_FUSION_ADAPT_STRUCT(
		extended_attribute,
		(std::string, key_)
		(std::string, value_)
)
#endif
////////////////////////////////////////////////
struct meta_data_ext4 {
	std::vector<extended_attribute> extended_attributes;
	int mode;
	full_time atime;
	full_time ctime;
	full_time mtime;
	int uid;
	std::string uname;
	int gid;
	std::string gname;
	size_t file_size;
};

BOOST_FUSION_ADAPT_STRUCT(
	meta_data_ext4,
	(std::vector<extended_attribute>,extended_attributes)
	(int,mode)
	(full_time,atime)
	(full_time,ctime)
	(full_time,mtime)
	(int,uid)
	(std::string,uname)
	(int,gid)
	(std::string,gname)
	(size_t,file_size)
)
////////////////////////////////////////////////

#endif /* Meta_Data_Ext4_hpp_ */
