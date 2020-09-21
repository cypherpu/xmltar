/*

XmltarGlobals.hpp
Copyright 2017-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: Apr 27, 2019
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

#ifndef SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_
#define SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_

#include <string>
#include <memory>
#include <queue>
#include <filesystem>

#include <fnmatch.h>
#include <sys/random.h>
#include <openssl/evp.h>

#include "Utilities/ExtendedPath.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Xmltar/XmltarOptions.hpp"

class XmltarMemberCreate;

class XmltarGlobals {
public:
	static const size_t xChaCha20Poly1305MessageLength=1<<15;

	XmltarOptions options_;
	std::string current_xmltar_file_name_;
	size_t current_volume_;
	std::unique_ptr<Snapshot> snapshot_;	// FIXME - make sure snapshot_ is destructed before options
	time_t invocationTime_;
	std::string salt_;

	std::priority_queue<ExtendedPath,std::vector<ExtendedPath>,std::greater<ExtendedPath>> filesToBeIncluded_;
	std::priority_queue<ExtendedPath,std::vector<ExtendedPath>,std::greater<ExtendedPath>> filesToBeExcluded_;

	std::string Sha3(std::filesystem::path filepath);
	std::string nextAction_;
	std::unique_ptr<XmltarMemberCreate> nextMember_;

	std::string passphrase_;
	std::string key_;

    int resultCode_;
    std::vector<std::string> errorMessages_;

    XmltarGlobals();
    size_t ArchiveDirectorySize();
    void NextMember();
    bool MatchesGlobs(std::filesystem::path p, std::vector<std::string> globs){
    	for(auto & s : globs){
    		int result=fnmatch(p.string().c_str(),s.c_str(),FNM_PATHNAME|FNM_PERIOD);
    		if (result==0) return true;
    		else if (result!=FNM_NOMATCH)
    			throw std::runtime_error("XmltarGlobals::MatchesGlobs: unknown error");
    	}

    	return false;
    }

    bool IncludedFile(ExtendedPath p){
    	return MatchesGlobs(p.path(),options_.sourceFileGlobs_);
    }

    bool ExcludedFile(ExtendedPath p){
    	return MatchesGlobs(p.path(),options_.excludeFileGlobs_);
    }

    void AddSubdirectories(std::filesystem::path const & p){
		std::filesystem::file_status f_stat=std::filesystem::symlink_status(p);

		if (std::filesystem::is_directory(f_stat)){
			for(auto & i : std::filesystem::directory_iterator(p) ){
				filesToBeIncluded_.push(ExtendedPath(i));
			}
		}
    }

    std::string InitializationVector(int nBytes){
    	std::string result(nBytes,' ');

    	if (getrandom(result.data(),nBytes,GRND_RANDOM)!=nBytes)
    		throw std::runtime_error("XmltarGlobals::InitializationVector: unable to getrandom");

    	return result;
    }

    std::string KeyFromPassphrase(std::string const & passphrase){
    	std::string key(32,' ');

		if (PKCS5_PBKDF2_HMAC(
				passphrase.data(), passphrase.size(),
				reinterpret_cast<unsigned const char *>(salt_.data()), salt_.size(),
				1000, EVP_sha3_512(), key.size(), reinterpret_cast<unsigned char *>(key.data())
			)!=1)
    		throw std::runtime_error("XmltarGlobals::KeyFromPassphrase: unable to getrandom");

		return key;
    }
};



#endif /* SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_ */
