/*
 * XmltarGlobals.cpp
 *
 *  Created on: Dec 14, 2019
 *      Author: dbetz
 */

#include "Xmltar/XmltarGlobals.hpp"
#include "Xmltar/XmltarMemberCreate.hpp"
#include "Generated/Utilities/Glob.hpp"
#include "Generated/Utilities/IsPrefixPath.hpp"

XmltarGlobals::XmltarGlobals()
	: current_xmltar_file_name_(), current_volume_(),
	  invocationTime_(time(nullptr)), resultCode_(0), errorMessages_() {

	if (options_.starting_volume_)
		current_volume_=options_.starting_volume_.get();
}

void XmltarGlobals::NextMember(){
	std::cerr << "XmltarArchive::NextMember(): entering" << std::endl;
	nextMember_.reset();

	if (filesToBeIncluded_.empty() && !globsToBeIncluded_.empty()){
		std::cerr << "XmltarArchive::NextMember(): replenish files to be included" << std::endl;
		while(filesToBeIncluded_.empty() && !globsToBeIncluded_.empty()){
			std::vector<std::string> tmp=BashGlob({globsToBeIncluded_[0]});
			for(auto & i : tmp)
				filesToBeIncluded_.push(std::filesystem::path(i));

			globsToBeIncluded_.erase(globsToBeIncluded_.begin());
		}

		if (filesToBeIncluded_.empty())
			return;

		if (snapshot_.get()!=nullptr)
			snapshot_->NewTemporarySnapshotFile();
	}

	filesToBeExcludedTruncated_=filesToBeExcludedComplete_;
	/*
	 * Files to be included in the archive are archived in command-line order.
	 * As files are included, we would like to erase excluded paths which
	 * could not possibly be relevant to any further included paths.
	 * as possible. It is not sufficient to merely erase excluded paths which
	 * are less than the current included path - we should not erase the excluded
	 * path "/bin" just because it is less than "/bin/foo"; we must also ensure
	 * the excluded path is not a path prefix of the included path.
	 */
	std::filesystem::path filepath;

	if (filesToBeIncluded_.empty()){
		nextMember_.reset(nullptr);
		return;
	}
	for( ; !filesToBeIncluded_.empty(); ){
		filepath=filesToBeIncluded_.top();
		filesToBeIncluded_.pop();
		std::cerr	<< "############ XmltarArchiveCreateSingleVolume::NextMember: "
					<< "considering " << filepath.string() << std::endl;
		while(!filesToBeExcludedTruncated_.empty() &&
				filesToBeExcludedTruncated_.top()<filepath &&
			!IsPrefixPath(filesToBeExcludedTruncated_.top(),filepath)){
			std::cerr	<< "############ XmltarArchiveCreateSingleVolume::NextMember: "
						<< "discarding exclude file " << filesToBeExcludedTruncated_.top().string() << std::endl;
			filesToBeExcludedTruncated_.pop();
		}

		std::cerr	<< "############ XmltarArchiveCreateSingleVolume::NextMember: "
					<< "filesToBeExcludedTruncated_.top()=" << (filesToBeExcludedTruncated_.size()?filesToBeExcludedTruncated_.top().string():"") << std::endl;

		if (filesToBeExcludedTruncated_.empty())
			break;
		if (IsPrefixPath(filesToBeExcludedTruncated_.top(),filepath)){
			std::cerr	<< "########### XmltarArchiveCreateSingleVolume::NextMember: "
						<< "discarding include file " << filepath << std::endl;
			continue;
		}
		else break;
	}

	std::cerr << "NextMember=" << filepath << std::endl;
	std::filesystem::file_status f_stat=std::filesystem::symlink_status(filepath);

	if (std::filesystem::is_directory(f_stat)){
		for(auto & p : std::filesystem::directory_iterator(filepath) ){
			filesToBeIncluded_.push(p);
		}
	}

	nextMember_.reset(new XmltarMemberCreate(*this,filepath));
}
