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

    std::unique_ptr<XmltarMemberCreate> nextMember_;

    int resultCode_;
    std::vector<std::string> errorMessages_;

    XmltarGlobals();
    void NextMember();
};



#endif /* SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_ */
