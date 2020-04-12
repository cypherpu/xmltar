/*

XmltarArchiveCreateMultiVolume.hpp
Copyright 2019-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: Feb 24, 2019
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

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVECREATEMULTIVOLUME_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVECREATEMULTIVOLUME_HPP_

#include "Xmltar/XmltarArchive.hpp"

class XmltarArchiveCreateMultiVolume : public XmltarArchive {
public:
	XmltarArchiveCreateMultiVolume(
		XmltarGlobals & globals,
		std::string filename
	);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVECREATEMULTIVOLUME_HPP_ */
