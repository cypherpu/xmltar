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
	std::string current_xmltar_file_name_;
	size_t current_volume_;
	std::unique_ptr<Snapshot> snapshot_;
	time_t invocationTime_;
	XmltarOptions options_;

	std::vector<std::filesystem::path> globsToBeIncluded_;
	std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,std::greater<std::filesystem::path>> filesToBeIncluded_;
	std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,std::greater<std::filesystem::path>> filesToBeExcludedComplete_;
	std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,std::greater<std::filesystem::path>> filesToBeExcludedTruncated_;

    std::unique_ptr<XmltarMemberCreate> nextMember_;

    XmltarGlobals(XmltarOptions const & options);
    void NextMember();
};



#endif /* SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_ */
