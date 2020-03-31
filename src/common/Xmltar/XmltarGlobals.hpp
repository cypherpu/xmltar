/*
 * XmltarGlobals.hpp
 *
 *  Created on: Apr 27, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_
#define SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_

#include <string>
#include <memory>
#include <queue>
#include <filesystem>

#include <fnmatch.h>

#include "Utilities/ExtendedPath.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Xmltar/XmltarOptions.hpp"

class XmltarMemberCreate;

class XmltarGlobals {
public:
	XmltarOptions options_;
	std::string current_xmltar_file_name_;
	size_t current_volume_;
	std::unique_ptr<Snapshot> snapshot_;	// FIXME - make sure snapshot_ is destructed before options
	time_t invocationTime_;

	std::priority_queue<ExtendedPath,std::vector<ExtendedPath>,std::greater<ExtendedPath>> filesToBeIncluded_;
	std::priority_queue<ExtendedPath,std::vector<ExtendedPath>,std::greater<ExtendedPath>> filesToBeExcluded_;

	std::string Sha3(std::filesystem::path filepath);
	std::string nextAction_;
	std::unique_ptr<XmltarMemberCreate> nextMember_;

    int resultCode_;
    std::vector<std::string> errorMessages_;

    XmltarGlobals();
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
};



#endif /* SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_ */
