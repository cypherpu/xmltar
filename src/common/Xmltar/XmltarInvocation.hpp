/*

XmltarInvocation.hpp
Copyright 2010-2020 by David A. Betz
David.A.Betz.MD@gmail.com
Created on: 2010
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

#ifndef COMMON_XMLTAR_XMLTARINVOCATION_HPP_

#define COMMON_XMLTAR_XMLTARINVOCATION_HPP_

#include <string>
#include <vector>
#include <queue>
#include <algorithm>

#include "Xmltar/XmltarOptions.hpp"
#include "Debug/Debug.hpp"
#include "Xmltar/XmltarGlobals.hpp"

class XmltarInvocation {
private:
	std::string version;
    XmltarGlobals & globals_;

public:
    XmltarInvocation(XmltarGlobals & globals);

    void MatchingPathsFromGlobs(
    	std::vector<std::string> const & patterns,
		std::priority_queue<ExtendedPath,std::vector<ExtendedPath>,std::greater<ExtendedPath>> *matchingPaths
	);
};

#endif // COMMON_XMLTAR_XMLTARINVOCATION_HPP_
