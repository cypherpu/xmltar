/*

XmltarArchive.hpp
Copyright 2017-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: Nov 21, 2017
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

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_

#include <queue>
#include <ios>
#include <fstream>
#include <iostream>

#include "Xmltar/XmltarOptions.hpp"
#include "Xmltar/XmltarGlobals.hpp"
#include "Xmltar/XmltarMemberCreate.hpp"

class XmltarArchiveHandler;
class XmltarMultiVolumeXmlHandler;
class XmltarSingleVolumeXmlHandler;

class PartialFileRead {
public:
	std::string filename_;
	std::streampos nextPositionToBeRead_;
};

class XmltarArchive {
protected:
	XmltarGlobals & globals_;
	std::string filename_;
	unsigned int volumeNumber_;
public:
	/**
	 * Single volume constructor - no volume number needed
	 */
	XmltarArchive(
		XmltarGlobals & globals,
		std::string filename
	);

	/**
	 * Multi volume constructor - requires volume number
	 */
	XmltarArchive(
		XmltarGlobals & globals,
		std::string filename,
		unsigned int volumeNumber
	);

	PartialFileRead create(unsigned int volumeNumber);
	PartialFileRead append(unsigned int volumeNumber);

	std::string ArchiveHeader(std::string filename, int archive_sequence_number);
	std::string ArchiveTrailerBegin();
	std::string ArchiveTrailerMiddle(unsigned int padding);
	std::string ArchiveTrailerEnd();

	std::string CompressedArchiveHeader(std::string filename, int archive_sequence_number);
	std::string CompressedArchiveTrailer();
	std::string CompressedArchiveTrailer(unsigned int desiredLength);

	bool static IsPaddingTrailer(std::string s);
	bool IsCompressedPaddingTrailer(std::fstream & iofs, std::ios::off_type offset);

	friend XmltarArchiveHandler;
	friend XmltarMultiVolumeXmlHandler;
	friend XmltarSingleVolumeXmlHandler;
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_ */
