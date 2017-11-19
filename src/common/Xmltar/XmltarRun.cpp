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

#include "Xmltar/XmltarRun.hpp"

#include <limits>

#include "Options/Options-TarStyle.hpp"

XmltarRun::XmltarRun(int argc, char const *argv[])
	: version("Xmltar_0_0_1") {

	options_.ProcessOptions(argc, argv);

	if (options_.verbosity_==3){
		std::cerr << "Argc=" << argc << std::endl;
		if (options_.operation_==XmltarOptions::APPEND) std::cerr << "Operation=APPEND" << std::endl;
		if (options_.operation_==XmltarOptions::CREATE) std::cerr << "Operation=CREATE" << std::endl;
		if (options_.operation_==XmltarOptions::LIST) std::cerr << "Operation=LIST" << std::endl;
		if (options_.operation_==XmltarOptions::EXTRACT) std::cerr << "Operation=EXTRACT" << std::endl;
		std::cerr << "verbosity=" << options_.verbosity_ << std::endl;
		if (options_.multi_volume_) std::cerr << "Multivolume" << std::endl;
		std::cerr << "starting_sequence_number=" << options_.starting_sequence_number_ << std::endl;
		std::cerr << "listed-incremental=" << options_.incremental_ << std::endl;
		std::cerr << "tape length=" << options_.tape_length_ << std::endl;
		std::cerr << "stop after=" << options_.stop_after_ << std::endl;
		std::cerr << "base_xmltar_file_name=" << options_.base_xmltar_file_name_ << std::endl;
		std::cerr << "Source file size=" << options_.source_files_.size() << std::endl;
		for(std::vector<boost::filesystem::path>::iterator i=options_.exclude_files_.begin(); i!=options_.exclude_files_.end(); ++i)
			std::cerr << "Exclude file=" << *i << std::endl;
		for(std::vector<boost::filesystem::path>::iterator i=options_.source_files_.begin(); i!=options_.source_files_.end(); ++i)
			std::cerr << "Source file=" << *i << std::endl;
		std::cerr << "listed-incremental file=" << options_.listed_incremental_file_ << std::endl;
		std::cerr << "files from=" << options_.files_from_ << std::endl;
	}
}
