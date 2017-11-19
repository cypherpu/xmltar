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

#ifndef Xmltar_hpp

#define Xmltar_hpp

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <memory>
#include <boost/filesystem.hpp>

#include "Xmltar/XmltarOptions.hpp"
#include "Meta_Data_Ext4.hpp"
#include "Debug.hpp"

class XmltarRun {
private:
	std::string version;
public:
	// options and arguments

    XmltarOptions options_;

	std::vector<boost::filesystem::path> file_stack;			// files waiting to be archived
	std::map<boost::filesystem::path,meta_data_ext4> directory_map;

	XmltarRun(int, char const *[]);

	bool Exclude_File(std::string);
	void Add_To_Archive(void);
	void Extract(void);
	bool Matches_Archive_Member(std::string);
};

#endif
