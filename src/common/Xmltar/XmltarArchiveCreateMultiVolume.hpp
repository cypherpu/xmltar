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
	std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,PathCompare> *filesToBeArchived_;
public:
	XmltarArchiveCreateMultiVolume(
		XmltarOptions & opts,
		std::string filename,
		// unsigned int volumeNumber,
		std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,PathCompare> *filesToBeArchived,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	);

	std::shared_ptr<XmltarMemberCreate> NextMember(){
		if (filesToBeArchived_->empty()){
			return std::shared_ptr<XmltarMemberCreate>();
		}

		std::filesystem::path const filepath=filesToBeArchived_->top();
		filesToBeArchived_->pop();
		std::filesystem::file_status f_stat=std::filesystem::symlink_status(filepath);

		if (std::filesystem::is_directory(f_stat)){
			for(auto & p : std::filesystem::directory_iterator(filepath) ){
				filesToBeArchived_->push(p);
			}
		}

		return std::make_shared<XmltarMemberCreate>(options_,filepath);
	}
};

#endif /* SRC_COMMON_XMLTAR_XMLTARARCHIVECREATEMULTIVOLUME_HPP_ */
