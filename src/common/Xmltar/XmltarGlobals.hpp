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
#include "Utilities/PathCompare.hpp"
#include "Options/XmltarOptions.hpp"

class XmltarGlobals {
public:
	std::string current_xmltar_file_name_;
	size_t current_volume_;
	std::unique_ptr<Snapshot> snapshot_;
	time_t invocationTime_;

	PathCompare pathCompare_;

	std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,std::greater<std::filesystem::path>> filesToBeIncluded_;
	std::priority_queue<std::filesystem::path,std::vector<std::filesystem::path>,std::greater<std::filesystem::path>> filesToBeExcluded_;

	XmltarGlobals(XmltarOptions const & options)
		: current_xmltar_file_name_(), current_volume_(),
		  invocationTime_(time(nullptr)) {

		if (options.starting_volume_)
			current_volume_=options.starting_volume_.get();
	}
};



#endif /* SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_ */
