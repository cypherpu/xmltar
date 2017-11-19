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

#include <boost/optional.hpp>

#include "Compression/Compression.hpp"
#include "Options/Options-TarStyle.hpp"

class XmltarOptions {
public:
    enum Operation { NOOP, APPEND, CREATE, LIST, EXTRACT };
	enum Encoding { BASE16, BASE64 };

	boost::optional<Operation> operation_;
	boost::optional<int> verbosity_;
	boost::optional<bool> multi_volume_;
	boost::optional<Compression> fileCompress_;
	boost::optional<Encoding> encoding_;
	boost::optional<Compression> archiveMemberCompress_;
	boost::optional<Compression> archiveCompress_;
	boost::optional<bool> incremental_;
	boost::optional<bool> compress_listed_incremental_file_;
	boost::optional<size_t> tape_length_;
	boost::optional<size_t> stop_after_;
	boost::optional<std::vector<boost::filesystem::path>> source_files_;
	boost::optional<boost::filesystem::path> listed_incremental_file_;
	boost::optional<boost::filesystem::path> files_from_;
	boost::optional<std::vector<boost::filesystem::path>> exclude_files_;
	boost::optional<std::string> archiveMemberTag_;
	boost::optional<bool> tabs_;
	boost::optional<bool> newlines_;

	boost::optional<std::string> base_xmltar_file_name_;		// xmltar_file;
	boost::optional<unsigned int> starting_sequence_number_;						// starting_volume;

	std::string current_xmltar_file_name_;
	unsigned int current_sequence_number_;

    XmltarOptions(void)
        : operation_(NOOP), verbosity_(0), multi_volume_(false), fileCompress_(IDENTITY), archiveMemberCompress_(IDENTITY), archiveCompress_(IDENTITY),
          encoding_(BASE16), incremental_(false), compress_listed_incremental_file_(false),
          tape_length_(std::numeric_limits<std::size_t>::max()), stop_after_(std::numeric_limits<std::size_t>::max()),
		  archiveMemberTag_(""), tabs_(true), newlines_(true), starting_sequence_number_(0), current_sequence_number_(0) { }

    void ProcessOptions(int argc, char const *argv[]);

	std::string Tabs(const char *tabStr){
		return tabs_?std::string(tabStr):std::string("");
	}

	std::string Newline(){
		return newlines_?std::string("\n"):std::string("");
	}

	std::string toXMLString();
};

#endif // XMLTAROPTIONS_HPP_
