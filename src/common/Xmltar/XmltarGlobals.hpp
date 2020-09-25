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

#include <spdlog/spdlog.h>

#include "Utilities/ExtendedPath.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Xmltar/XmltarOptions.hpp"

class XmltarMemberCreate;

std::string KeyFromPassphrase(std::string const & passphrase, std::string const & salt);

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
    bool MatchesGlobs(std::filesystem::path p, std::vector<std::string> globs);
    bool IncludedFile(ExtendedPath p);
    bool ExcludedFile(ExtendedPath p);
    void AddSubdirectories(std::filesystem::path const & p);
    std::string InitializationVector(int nBytes);
    std::string KeyFromPassphrase(std::string const & passphrase);
};



#endif /* SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_ */
