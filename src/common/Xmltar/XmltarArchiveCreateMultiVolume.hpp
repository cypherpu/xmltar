/*
 * XmltarArchiveCreateMultiVolume.hpp
 *
 *  Created on: Feb 24, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVECREATEMULTIVOLUME_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVECREATEMULTIVOLUME_HPP_

#include "Xmltar/XmltarArchive.hpp"

class XmltarArchiveCreateMultiVolume : public XmltarArchive {
public:
	XmltarArchiveCreateMultiVolume(
		XmltarOptions & opts,
		std::string filename,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	);

	std::shared_ptr<XmltarMemberCreate> NextMember(){
		if (options_.filesToBeIncluded_.empty()){
			return std::shared_ptr<XmltarMemberCreate>();
		}

		std::filesystem::path const filepath=options_.filesToBeIncluded_.top();
		options_.filesToBeIncluded_.pop();
		std::filesystem::file_status f_stat=std::filesystem::symlink_status(filepath);

		if (std::filesystem::is_directory(f_stat)){
			for(auto & p : std::filesystem::directory_iterator(filepath) ){
				options_.filesToBeIncluded_.push(p);
			}
		}

		return std::make_shared<XmltarMemberCreate>(options_,filepath);
	}
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVECREATEMULTIVOLUME_HPP_ */
