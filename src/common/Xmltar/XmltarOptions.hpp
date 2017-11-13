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

#ifndef XmltarOptions_hpp_

#define XmltarOptions_hpp_

#include <limits>

class XmltarOptions {
public:
    enum Operation { NOOP, APPEND, CREATE, LIST, EXTRACT };
    enum Compression { IDENTITY, GZIP, BZIP2, LZIP };
	enum Encoding { BASE16, BASE64 };

	Operation operation;
	int verbosity;
	bool multi_volume;
	Compression fileCompress;
	Compression archiveMemberCompress;
	Compression archiveCompress;
	Encoding encoding;
	bool incremental;
	bool compress_listed_incremental_file;
	size_t tape_length;
	size_t stop_after;
	std::vector<boost::filesystem::path> source_files;
	boost::filesystem::path listed_incremental_file;
	boost::filesystem::path files_from;
	std::vector<boost::filesystem::path> exclude_files;
	std::string tag;
   	bool tabs_;
   	bool newlines_;

	std::string base_xmltar_file_name;							// xmltar_file;
	unsigned int starting_sequence_number;						// starting_volume;

	std::string current_xmltar_file_name;
	unsigned int current_volume_number;

    XmltarOptions(void)
        : operation(NOOP), verbosity(0), multi_volume(false), fileCompress(IDENTITY), archiveMemberCompress(IDENTITY), archiveCompress(IDENTITY),
          encoding(BASE16), incremental(false), compress_listed_incremental_file(false),
          tape_length(std::numeric_limits<std::size_t>::max()), stop_after(std::numeric_limits<std::size_t>::max()),
          tag(""), tabs_(true), newlines_(true), current_volume_number(0) { }

    std::string HeaderMagicNumber(void){
		switch(archiveCompress){
		case IDENTITY:
			return std::string("<?xml");
		case GZIP:
			return std::string("\x1f\x8b");
		case BZIP2:
			return std::string("BZh");
		case LZIP:
			return std::string("\x4c\x5a\x49\x50\x01");
		default:
			throw "XmltarOptions::HeaderMagicNumber: unrecognized Compression";
		}
	}

    std::string TrailerMagicNumber(void){
		switch(archiveCompress){
		case IDENTITY:
			return std::string("</members");
		case GZIP:
			return std::string("\x1f\x8b");
		case BZIP2:
			return std::string("BZh");
		case LZIP:
			return std::string("\x4c\x5a\x49\x50\x01");
		default:
			throw "XmltarOptions::TrailerMagicNumber: unrecognized Compression";
		}
	}

	std::string Tabs(const char *tabStr){
		return tabs_?std::string(tabStr):std::string("");
	}

	std::string Newline(){
		return newlines_?std::string("\n"):std::string("");
	}
};

#endif
