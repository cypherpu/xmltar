/*

Copyright 2010 by David A. Betz

This file is part of xmltar.

xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xmltar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef XMLTAROPTIONS_HPP_

#define XMLTAROPTIONS_HPP_

#include <limits>
#include <fstream>

#include <boost/optional.hpp>

#include "Options/Options-TarStyle.hpp"
#include "Transform/Transform.hpp"
#include "Transform/TransformIdentity.hpp"
#include "Transform/TransformGzip.hpp"
#include "Transform/TransformBzip2.hpp"
#include "Transform/TransformLzip.hpp"
#include "Transform/TransformHex.hpp"
#include "Snapshot/Snapshot.hpp"

class XmltarOptions {
public:
    enum Operation { NOOP, APPEND, CREATE, LIST, EXTRACT };
	enum Encoding { BASE16, BASE64 };

	boost::optional<Operation> operation_;
	boost::optional<int> verbosity_;
	boost::optional<bool> multi_volume_;
	std::shared_ptr<Transform> fileCompression_;
	std::shared_ptr<Transform> encoding_;
	std::shared_ptr<Transform> archiveMemberCompression_;
	std::shared_ptr<Transform> archiveCompression_;
	boost::optional<size_t> tape_length_;
	boost::optional<size_t> stop_after_;
	boost::optional<std::filesystem::path> listed_incremental_file_;
	std::shared_ptr<Transform> incrementalFileCompression_;
	boost::optional<unsigned int> dump_level_;
	boost::optional<std::filesystem::path> files_from_;
	boost::optional<std::vector<std::filesystem::path>> exclude_files_;
	boost::optional<std::string> archiveMemberTag_;
	boost::optional<bool> tabs_;
	boost::optional<bool> newlines_;
	boost::optional<std::string> base_xmltar_file_name_;		// xmltar_file;
	boost::optional<unsigned int> starting_sequence_number_;						// starting_volume;
	boost::optional<std::vector<std::filesystem::path>> source_files_;

	std::string current_xmltar_file_name_;
	unsigned int current_sequence_number_;
	boost::optional<Snapshot> snapshot_;
	std::shared_ptr<std::ofstream> incrementalFileOfs_;
	time_t invocationTime_;

    XmltarOptions(void)
        : operation_(), verbosity_(), multi_volume_(),
		  fileCompression_(new TransformIdentity("fileCompression")),
		  encoding_(new TransformHex("encoding")),
		  archiveMemberCompression_(new TransformIdentity("archiveMemberCompression")),
		  archiveCompression_(new TransformIdentity("archiveCompression")),
          tape_length_(), stop_after_(),
		  listed_incremental_file_(),
		  incrementalFileCompression_(new TransformIdentity("listed-incremental-compression")),
		  dump_level_(),
		  files_from_(), exclude_files_(),
		  archiveMemberTag_(),tabs_(), newlines_(),
		  base_xmltar_file_name_(), starting_sequence_number_(), source_files_(),
		  current_xmltar_file_name_(), current_sequence_number_(), incrementalFileOfs_(),
		  invocationTime_(time(nullptr)){ }

    void ProcessOptions(int argc, char const *argv[]);

	std::string Tabs(char const *tabStr) const {
		if (tabs_)
			return tabs_.get()?std::string(tabStr):std::string("");
		else return std::string(tabStr);
	}

	std::string Newline() const {
		if (newlines_)
			return newlines_.get()?std::string("\n"):std::string("");
		else return std::string("\n");
	}

	std::string toXMLString();
};

#endif // XMLTAROPTIONS_HPP_
