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

	std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,std::greater<std::filesystem::path>> filesToBeIncluded_;
	std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,std::greater<std::filesystem::path>> filesToBeExcluded_;

	void NextMemberAux(std::filesystem::path filepath);
	std::unique_ptr<XmltarMemberCreate> nextMember_;

    int resultCode_;
    std::vector<std::string> errorMessages_;

    XmltarGlobals();
    void NextMemberAux();
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

    bool IncludedFile(std::filesystem::path p){
    	return MatchesGlobs(p,options_.sourceFileGlobs_);
    }

    bool ExcludedFile(std::filesystem::path p){
    	return MatchesGlobs(p,options_.excludeFileGlobs_);
    }
};



#endif /* SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_ */
