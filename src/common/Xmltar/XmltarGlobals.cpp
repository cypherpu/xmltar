/*

XmltarGlobals.cpp
Copyright 2017-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: Dec 14, 2019
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

#include "Xmltar/XmltarGlobals.hpp"
#include "Xmltar/XmltarMemberCreate.hpp"
#include "Generated/Utilities/Glob.hpp"
#include "Generated/Utilities/IsPrefixPath.hpp"
#include "Utilities/Sha3.hpp"

#if DEBUG
#define THROW(x,y)	do { std::cerr << __FILE__ << ":" << __func__ << ":" << __LINE__ << ":" << y << std::endl; } while(0)
#else
#define THROW(x,y)	throw x(y)
#endif

const size_t XmltarGlobals::xChaCha20Poly1305MessageLength;

std::string KeyFromPassphrase(std::string const & passphrase, std::string const & salt){
	std::string key(32,' ');

	if (PKCS5_PBKDF2_HMAC(
			passphrase.data(), passphrase.size(),
			reinterpret_cast<unsigned const char *>(salt.data()), salt.size(),
			1000, EVP_sha3_512(), key.size(), reinterpret_cast<unsigned char *>(key.data())
		)!=1)
		throw std::runtime_error("XmltarGlobals::KeyFromPassphrase: unable to getrandom");

	return key;
}

XmltarGlobals::XmltarGlobals()
	: current_xmltar_file_name_(), current_volume_(),
	  invocationTime_(time(nullptr)), key_(), resultCode_(0), errorMessages_() {

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

size_t  XmltarGlobals::ArchiveDirectorySize(){
	std::filesystem::path p(options_.base_xmltar_file_name_.value());
	std::filesystem::path archiveDirectory;

	if (p.has_root_path()) archiveDirectory=p.parent_path();			// absolute path
	else if (p.has_parent_path()) archiveDirectory=p.parent_path();		// relative path with at least one directory component
	else archiveDirectory=std::filesystem::current_path();				// only filename was specified

	size_t result=0;
    for(auto& p: std::filesystem::directory_iterator(archiveDirectory))
    	result+=p.file_size();

    return result;
}

void XmltarGlobals::NextMember(){
	std::cerr << "XmltarArchive::NextMember(): entering" << std::endl;
#if 0
	while(ArchiveDirectorySize()>options_.wait_for_space_.value()){
		std::cerr << "Waiting for archive directory to empty" << std::endl;
		sleep(10);
	}
#endif
	std::string sha3_512;

	if (nextMember_){
		if (std::filesystem::is_regular_file(nextMember_->f_stat_) && options_.sha3_512_){
			sha3_512=nextMember_->sha3sum512_.ForceWriteAndClose("");
		}
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
			snapshot_->CopyFrontFileEntryAndPop();
		}

		AddSubdirectories(nextMember_->filepath_);
	}
	nextMember_.reset(nullptr);

	std::filesystem::path filepath;

	if (snapshot_){
		for(;;){
			if (snapshot_->fileEntries_.empty())
				snapshot_->ReplenishFileEntries();

			while(filesToBeExcluded_.top()<snapshot_->fileEntries_.front().pathname_
					&& filesToBeExcluded_.top()<filesToBeIncluded_.top())
				filesToBeExcluded_.pop();

			std::cerr << "NextMember: " << snapshot_->fileEntries_.front().pathname_ << " " << filesToBeIncluded_.top() << std::endl;

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
							|| snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).action_=="created"){
						nextAction_="modified";
						nextMember_.reset(new XmltarMemberCreate(*this,filesToBeIncluded_.top().path()));

						std::cerr
							<< snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).modificationTime_ << " "
							<< nextMember_->stat_buf_.st_mtim.tv_sec << " "
							<< snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).size_ << " "
							<< nextMember_->stat_buf_.st_size
							<< std::endl;



						if (snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).modificationTime_
								!=nextMember_->stat_buf_.st_mtim.tv_sec
							|| snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).size_
								!=nextMember_->stat_buf_.st_size
							|| (options_.sha3_512_
								&& snapshot_->fileEntries_.front().LastEvent(options_.dump_level_.get()).sha3_512_
									!=Sha3(filesToBeIncluded_.top().path()))){
							filesToBeIncluded_.pop();
							return;
						}
						else {
							AddSubdirectories(filesToBeIncluded_.top().path());
							filesToBeIncluded_.pop();
							snapshot_->CopyFrontFileEntryAndPop();
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
				if (filesToBeIncluded_.top().pathType()==ExtendedPath::PathType::MAX
					&& filesToBeExcluded_.top().pathType()==ExtendedPath::PathType::MAX)
					return;
				else if (filesToBeIncluded_.top().pathType()==ExtendedPath::PathType::MAX
					|| filesToBeExcluded_.top().pathType()==ExtendedPath::PathType::MAX)
					throw std::logic_error("XmltarGlobals::NextMember: both paths should be MAX 2");

				filesToBeIncluded_.pop();
				filesToBeExcluded_.pop();
			}
		}
	}
}

bool XmltarGlobals::MatchesGlobs(std::filesystem::path p, std::vector<std::string> globs){
	for(auto & s : globs){
		int result=fnmatch(p.string().c_str(),s.c_str(),FNM_PATHNAME|FNM_PERIOD);
		if (result==0) return true;
		else if (result!=FNM_NOMATCH)
			throw std::runtime_error("XmltarGlobals::MatchesGlobs: unknown error");
	}

	return false;
}

bool XmltarGlobals::IncludedFile(ExtendedPath p){
	return MatchesGlobs(p.path(),options_.sourceFileGlobs_);
}

bool XmltarGlobals::ExcludedFile(ExtendedPath p){
	return MatchesGlobs(p.path(),options_.excludeFileGlobs_);
}

void XmltarGlobals::AddSubdirectories(std::filesystem::path const & p){
	std::filesystem::file_status f_stat=std::filesystem::symlink_status(p);

	if (std::filesystem::is_directory(f_stat)){
		for(auto & i : std::filesystem::directory_iterator(p) ){
			filesToBeIncluded_.push(ExtendedPath(i));
		}
	}
}

std::string XmltarGlobals::InitializationVector(int nBytes){
	std::string result(nBytes,' ');

	if (getrandom(result.data(),nBytes,GRND_RANDOM)!=nBytes)
		throw std::runtime_error("XmltarGlobals::InitializationVector: unable to getrandom");

	return result;
}

std::string XmltarGlobals::KeyFromPassphrase(std::string const & passphrase){
	return ::KeyFromPassphrase(passphrase, salt_);
}

