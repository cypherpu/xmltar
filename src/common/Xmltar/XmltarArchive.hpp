/*
 * XmltarArchive.hpp
 *
 *  Created on: Nov 21, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVE_HPP_

#include <queue>
#include <ios>
#include <fstream>
#include <iostream>

#include "Xmltar/XmltarOptions.hpp"
#include "Xmltar/XmltarGlobals.hpp"
#include "Utilities/PathCompare.hpp"
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
	XmltarOptions const & options_;
	XmltarGlobals & globals_;
	std::string filename_;
	unsigned int volumeNumber_;
	std::shared_ptr<XmltarMemberCreate> & nextMember_;
public:
	/**
	 * Single volume constructor - no volume number needed
	 */
	XmltarArchive(
		XmltarOptions const & opts,
		XmltarGlobals & globals,
		std::string filename,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	);

	/**
	 * Multi volume constructor - requires volume number
	 */
	XmltarArchive(
		XmltarOptions const & opts,
		XmltarGlobals & globals,
		std::string filename,
		unsigned int volumeNumber,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	);

	std::shared_ptr<XmltarMemberCreate> NextMember();

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
