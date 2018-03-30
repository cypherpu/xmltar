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

#ifndef COMMON_XMLTAR_XMLTARINVOCATION_HPP_

#define COMMON_XMLTAR_XMLTARINVOCATION_HPP_

#include <string>
#include <vector>
#include <queue>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "Options/XmltarOptions.hpp"
#include "Meta_Data_Ext4.hpp"
#include "Debug.hpp"
#include "Utilities/PathCompare.hpp"

class XmltarInvocation {
private:
	std::string version;
    XmltarOptions options_;
    PathCompare pathCompare;
    // std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> filesToBeArchived_;

public:
    XmltarInvocation(XmltarOptions const & options);

	bool Exclude_File(std::string);
	void Add_To_Archive(void);
	void Extract(void);
	bool Matches_Archive_Member(std::string);
	XmltarOptions const & Options(){ return options_; }
};

#endif // COMMON_XMLTAR_XMLTARINVOCATION_HPP_
