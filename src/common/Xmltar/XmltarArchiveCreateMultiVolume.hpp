/*
 * XmltarArchiveCreateMultiVolume.hpp
 *
 *  Created on: Feb 24, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARARCHIVECREATEMULTIVOLUME_HPP_
#define SRC_COMMON_XMLTAR_XMLTARARCHIVECREATEMULTIVOLUME_HPP_

#include "Xmltar/XmltarArchive.hpp"
#include "Utilities/IsPrefixPath.hpp"

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

		/*
		 * Files to be included in the archive are archived in path order.
		 * As files are included, we would like to erase excluded paths which
		 * could not possibly be relevant to any further included paths.
		 * as possible. It is not sufficient to merely erase excluded paths which
		 * are less than the current included path - we should not erase the excluded
		 * path "/bin" just because it is less than "/bin/foo"; we must also ensure
		 * the excluded path is not a path prefix of the included path.
		 */
		std::filesystem::path filepath;
		for( ; ; ){
			filepath=options_.filesToBeIncluded_.top();
			options_.filesToBeIncluded_.pop();
			while(options_.filesToBeExcluded_.top()<filepath &&
				!IsPrefixPath(options_.filesToBeExcluded_.top(),filepath))
				options_.filesToBeExcluded_.pop();

			if (IsPrefixPath(options_.filesToBeExcluded_.top(),filepath))
				options_.filesToBeIncluded_.pop();
			else break;
		}

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
