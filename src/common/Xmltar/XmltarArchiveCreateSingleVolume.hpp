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
		XmltarOptions const & opts,
		XmltarGlobals & globals,
		std::string filename,
		unsigned int volumeNumber,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	);
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVECREATESINGLEVOLUME_HPP_ */
