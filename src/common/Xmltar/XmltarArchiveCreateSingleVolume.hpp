/*
 * XmltarArchiveCreateSingleVolume.hpp
 *
 *  Created on: Feb 24, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVECREATESINGLEVOLUME_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVECREATESINGLEVOLUME_HPP_

#include "Xmltar/XmltarArchive.hpp"

class XmltarArchiveCreateSingleVolume : public XmltarArchive {
public:
	XmltarArchiveCreateSingleVolume(
		XmltarOptions & opts,
		std::string filename,
		unsigned int volumeNumber,
		std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,PathCompare> *filesToBeArchived,
		std::shared_ptr<XmltarMember> & nextMember
	);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVECREATESINGLEVOLUME_HPP_ */
