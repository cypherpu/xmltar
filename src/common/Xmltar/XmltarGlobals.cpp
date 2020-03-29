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
#include "Utilities/Sha3.hpp"

XmltarGlobals::XmltarGlobals()
	: current_xmltar_file_name_(), current_volume_(),
	  invocationTime_(time(nullptr)), resultCode_(0), errorMessages_() {

	if (options_.starting_volume_)
		current_volume_=options_.starting_volume_.get();
}

std::string XmltarGlobals::Sha3(std::filesystem::path filepath){
	Sha3_512 sha;
	std::ifstream ifs;

	sha.Open();
	ifs.open(filepath);
	if (!ifs)
		throw std::runtime_error("XmltarGlobals::Sha3: unable to open file");

	while(ifs){
		char buffer[1024];

		ifs.read(buffer,sizeof(buffer)/sizeof(char));
		sha.ForceWrite(std::string(buffer,ifs.gcount()));
	}

	return sha.ForceWriteAndClose("");
}

void XmltarGlobals::NextMember(){
	std::cerr << "XmltarArchive::NextMember(): entering" << std::endl;

	std::string sha3_512;

	if (std::filesystem::is_regular_file(nextMember_->f_stat_) && options_.sha3_512_){
		sha3_512=nextMember_->sha3sum512_.ForceWriteAndClose("");
	}
	if (nextMember_){
		if (snapshot_){
			snapshot_->fileEntries_.front().snapshotEvents_.push_back(
				SnapshotEvent(
					invocationTime_,
					options_.dump_level_.get(),
					nextAction_,
					nextMember_->startingVolumeName_,
					nextMember_->stat_buf_.st_mtim.tv_sec,
					nextMember_->stat_buf_.st_size,
					sha3_512
				)
			);
		}

		std::filesystem::file_status f_stat=std::filesystem::symlink_status(nextMember_->filepath_);

		if (std::filesystem::is_directory(f_stat)){
			for(auto & p : std::filesystem::directory_iterator(nextMember_->filepath_) ){
				filesToBeIncluded_.push(ExtendedPath(p));
			}
		}
	}
	nextMember_.reset(nullptr);

#if 0
		}
		if (options_.listed_incremental_file_){

			snapshot_->newSnapshotFileOfs_
				<< "\t<file name=\"" << EncodeStringToXMLSafeString(nextMember_->filepath_.string()) << "\">\n";
			snapshot_->newSnapshotFileOfs_
				<< "\t\t" << SnapshotEvent(
								invocationTime_,
								options_.dump_level_.get(),
								"add",
								nextMember_->startingVolumeName_,
								nextMember_->stat_buf_.st_mtim.tv_sec,
								nextMember_->stat_buf_.st_size,
								sha3_512)
							<< std::endl
				<< "\t</file>" << std::endl;
			std::cerr << "Digest=" << sha3_512 << std::endl;
		}
	}
#endif

	std::filesystem::path filepath;

	if (snapshot_){
		for(;;){
			if (snapshot_->fileEntries_.empty())
				snapshot_->ReplenishFileEntries();

			while(filesToBeExcluded_.top()<snapshot_->fileEntries_.front().pathname_
					&& filesToBeExcluded_.top()<filesToBeIncluded_.top())
				filesToBeExcluded_.pop();

			if (snapshot_->fileEntries_.front().pathname_<filesToBeIncluded_.top()){
				if (IncludedFile(snapshot_->fileEntries_.front().pathname_)
					&& !ExcludedFile(snapshot_->fileEntries_.front().pathname_))
					if (snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).action_!="deleted")
						snapshot_->fileEntries_.front().snapshotEvents_.push_back(
							SnapshotEvent(
								invocationTime_,
								options_.dump_level_.get(),
								"deleted",
								"",
								0,
								0,
								""
							)
						);
				snapshot_->CopyFrontFileEntryAndPop();
			}
			else if (snapshot_->fileEntries_.front().pathname_>filesToBeIncluded_.top()){
				if (filesToBeIncluded_.top()==filesToBeExcluded_.top()){
					filesToBeIncluded_.pop();
				}
				else {
					snapshot_->fileEntries_.push_front(SnapshotFileEntry(filesToBeIncluded_.top()));
					nextAction_="created";
					nextMember_.reset(new XmltarMemberCreate(*this,filesToBeIncluded_.top().path()));
					filesToBeIncluded_.pop();
					return;
				}
			}
			else {
				if (snapshot_->fileEntries_.front().pathname_.pathType()==ExtendedPath::PathType::MAX)
					if (filesToBeIncluded_.top().pathType()==ExtendedPath::PathType::MAX)
						return;
					else
						throw std::logic_error("XmltarGlobals::NextMember: both paths should be MAX 1");

				if (ExcludedFile(filesToBeIncluded_.top())){
					snapshot_->CopyFrontFileEntryAndPop();
					filesToBeIncluded_.pop();
				}
				else {
					if (snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).action_=="deleted"){
						nextAction_="created";
						nextMember_.reset(new XmltarMemberCreate(*this,filesToBeIncluded_.top().path()));
						filesToBeIncluded_.pop();
						snapshot_->fileEntries_.pop_front();
						return;
					}
					else if (snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).action_=="modified"
							|| snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).action_=="added"){
						nextAction_="modified";
						nextMember_.reset(new XmltarMemberCreate(*this,filesToBeIncluded_.top().path()));
						if (snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).modificationTime_
								!=nextMember_->stat_buf_.st_mtim.tv_sec
							|| snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).size_
								!=nextMember_->stat_buf_.st_size
							|| (options_.sha3_512_
								&& snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).sha3_512_
									!=Sha3(filesToBeIncluded_.top().path()))){
							filesToBeIncluded_.pop();
							snapshot_->fileEntries_.pop_front();
							return;
						}
						else {
							filesToBeIncluded_.pop();
							snapshot_->fileEntries_.pop_front();
							nextMember_.reset(nullptr);
						}
					}
				}
			}
		}
	}
	else {
		for(;;){
			while(filesToBeExcluded_.top()<filesToBeIncluded_.top())
				filesToBeExcluded_.pop();

			if (filesToBeIncluded_.top()<filesToBeExcluded_.top()){
				filepath=filesToBeIncluded_.top().path();
				filesToBeIncluded_.pop();
				nextMember_.reset(new XmltarMemberCreate(*this,filepath));
				return;
			}
			else {
				if (filesToBeIncluded_.top().pathType()==ExtendedPath::PathType::MAX)
					if (filesToBeExcluded_.top().pathType()==ExtendedPath::PathType::MAX)
						return;
					else
						throw std::logic_error("XmltarGlobals::NextMember: both paths should be MAX 2");

				filesToBeIncluded_.pop();
				filesToBeExcluded_.pop();
			}
		}
	}
}
