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

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <boost/format.hpp>
#include <memory>

#include "Archive.hpp"
#include "Archive_Member.hpp"
#include "Conversions.hpp"
#include "Incremental_File.hpp"
#include "XmltarRun.hpp"

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
}

/*
 * Add_To_Archive - called either from:
 * 1. Create, in which case bytes_already_written=0
 * 2. Append, in which case bytes_already_written=the number of bytes already written by a previous invocation of xmltar
 */

void XmltarRun::Add_To_Archive(void){
    DEBUGCXX(debugcxx,"Xmltar::Add_To_Archive");
	size_t volumes_completed(0);
	int sequence_number(options_.starting_sequence_number_.get());

	boost::scoped_ptr<Archive> archive(new Archive(options_,sequence_number));

    if (options_.operation_==XmltarOptions::CREATE){
        archive->Attach();
        archive->Write_Archive_Header();
    }

	Incremental_File ifs;
	if (options_.incremental_) ifs.Open(options_.listed_incremental_file_.get().string(),options_.compress_listed_incremental_file_.get());

	while(options_.source_files_.get().size()!=0){
	    boost::filesystem::path current_file(options_.source_files_.get().back());
	    options_.source_files_.get().pop_back();

		if (boost::filesystem::is_directory(current_file))
			for(boost::filesystem::directory_iterator it(current_file); it!=boost::filesystem::directory_iterator(); ++it)
				if (!Exclude_File(it->path().string())) options_.source_files_.get().push_back(*it);
				else std::cerr << "Excluding file at " << *it << std::endl;

        Archive_Member member(current_file, options_);

		if (options_.incremental_)
			if (ifs.Is_Current(member))
				continue;

		if (options_.verbosity_>=1)
		    std::cerr << current_file << std::endl;

		volumes_completed=archive->Store_Member(member);

		if (options_.incremental_)
			ifs.Update(member);

		if (volumes_completed>=options_.stop_after_) break;
	}

	archive->Close(true);

	if (options_.incremental_){
		ifs.Write();
		ifs.Close();
	}
}
