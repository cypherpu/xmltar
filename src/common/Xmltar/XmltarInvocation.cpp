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

#include <limits>

#include <boost/format.hpp>
#include <boost/random.hpp>
#include <stdexcept>

#include "Options/Options-TarStyle.hpp"
#include "Snapshot/Snapshot.hpp"

#include "XmltarInvocation.hpp"
#include "XmltarMember.hpp"
#include "Xmltar/XmltarArchive.hpp"

XmltarInvocation::XmltarInvocation(XmltarOptions const & options)
	: version("Xmltar_0_0_1"), options_(options), pathCompare() {

	if (options_.verbosity_==3){
		if (options_.operation_==XmltarOptions::APPEND) std::cerr << "Operation=APPEND" << std::endl;
		if (options_.operation_==XmltarOptions::CREATE) std::cerr << "Operation=CREATE" << std::endl;
		if (options_.operation_==XmltarOptions::LIST) std::cerr << "Operation=LIST" << std::endl;
		if (options_.operation_==XmltarOptions::EXTRACT) std::cerr << "Operation=EXTRACT" << std::endl;
		std::cerr << "verbosity=" << options_.verbosity_.get() << std::endl;
		if (options_.multi_volume_) std::cerr << "Multivolume" << std::endl;
		std::cerr << "starting_sequence_number=" << options_.starting_sequence_number_.get() << std::endl;
		std::cerr << "tape length=" << options_.tape_length_.get() << std::endl;
		std::cerr << "stop after=" << options_.stop_after_.get() << std::endl;
		std::cerr << "base_xmltar_file_name=" << options_.base_xmltar_file_name_.get() << std::endl;
		std::cerr << "Source file size=" << options_.source_files_.get().size() << std::endl;
		if (options_.exclude_files_)
			for(std::vector<boost::filesystem::path>::iterator i=options_.exclude_files_.get().begin(); i!=options_.exclude_files_.get().end(); ++i)
				std::cerr << "Exclude file=" << *i << std::endl;
		if (options_.source_files_)
			for(std::vector<boost::filesystem::path>::iterator i=options_.source_files_.get().begin(); i!=options_.source_files_.get().end(); ++i)
				std::cerr << "Source file=" << *i << std::endl;
		std::cerr << "listed-incremental file=" << options_.listed_incremental_file_.get() << std::endl;
		std::cerr << "files from=" << options_.files_from_.get() << std::endl;
	}

	std::priority_queue<boost::filesystem::path,std::vector<boost::filesystem::path>,PathCompare> filesToArchive(pathCompare);
	if (options_.source_files_)
		for(auto & i : options_.source_files_.get())
			filesToArchive.push(i);
	else if (options_.files_from_){
		std::ifstream ifs(options_.files_from_.get().string());

		if (ifs){
			std::string line;
			while(std::getline(ifs,line))
				filesToArchive.push(boost::filesystem::path(line));
		}
		else
			throw std::runtime_error("XmltarInvocation::XmltarInvocation: cannot open files_from");
	}
	else
		throw std::runtime_error("XmltarInvocation::XmltarInvocation: no files specified");

	boost::optional<Snapshot> snapshot;
	if (options_.listed_incremental_file_){
		snapshot=Snapshot();
		if (boost::filesystem::exists(options_.listed_incremental_file_.get())){
			snapshot.get().
					Load(options_.listed_incremental_file_.get().string());
		}
	}

	if (options_.operation_ && options_.operation_==XmltarOptions::CREATE){
		if (options_.multi_volume_){
			if (!options_.starting_sequence_number_)
				throw std::logic_error("XmltarRun::XmltarRun: must specify starting sequence number to create multivolume archive");

            if (!options_.stop_after_) options_.stop_after_=std::numeric_limits<size_t>::max();
            size_t volumeNumber=options_.starting_sequence_number_.get();
            std::shared_ptr<XmltarMember> nextMember;

            for(unsigned int i=0; i<options_.stop_after_.get(); ++i, ++volumeNumber){
                boost::format fmt(options_.base_xmltar_file_name_.get());
                fmt % volumeNumber;
                std::string filename=str(fmt);

                XmltarArchive xmltarArchive(options_,filename, volumeNumber, filesToArchive, nextMember);
                std::cerr << "*****************" << std::endl;
                // We return from XmltarArchive under 2 circumstances:
                // 1. we ran out of files to archive
                // 2. we ran out of space in the archive

            	if (xmltarArchive.ranOutOfFiles()) break;
            }
		}
		else {	// !options_.multi_volume_
			if (!options_.base_xmltar_file_name_)
				throw std::runtime_error("xmltar: XmltarInvocation: must specify an output file");

            std::shared_ptr<XmltarMember> nextMember;
            XmltarArchive xmltarArchive(options_,options_.base_xmltar_file_name_.get(), 0, filesToArchive,nextMember);
		}
	}
	else if (options_.operation_ && options_.operation_==XmltarOptions::APPEND){
		/*
		if (options_.multi_volume_){
			if (!options_.starting_sequence_number_){
				throw std::logic_error("XmltarRun::XmltarRun: must specify starting sequence number to create multivolume archive");
			}

            if (!options_.stop_after_) options_.stop_after_=std::numeric_limits<size_t>::max();
            size_t volumeNumber=options_.starting_sequence_number_.get();
            for(unsigned int i=0; i<options_.stop_after_.get(); ++i, ++volumeNumber){
                boost::format fmt(options_.base_xmltar_file_name_.get());
                fmt % volumeNumber;
                std::string filename=str(fmt);

                XmltarArchive xmltarArchive(options_,filename, volumeNumber, filesToArchive,std::streampos(0));

            	if (xmltarArchive.ranOutOfFiles()) break;
            }
		}
		else {
            XmltarArchive xmltarArchive(options_,options_.base_xmltar_file_name_.get(), 0, filesToArchive, std::streampos(0));
		}
		*/
	}

#if 0
	if (options_.multi_volume_){
		if (options_.operation_ && options_.operation_==XmltarOptions::APPEND){
			if (!options_.starting_sequence_number_){
				throw std::logic_error("XmltarRun::XmltarRun: must specify starting sequence number to append to multivolume archive");
			}
			size_t lastArchiveSequenceNumber=options_.starting_sequence_number_.get();
			for( ; ; ++lastArchiveSequenceNumber){
	            boost::format fmt(options_.base_xmltar_file_name_.get());
	            fmt % lastArchiveSequenceNumber;
	            std::string filename=str(fmt);
	            if (boost::filesystem::exists(boost::filesystem::path(filename))){
	            	for( ; ; ++lastArchiveSequenceNumber){
	    	            boost::format fmt(options_.base_xmltar_file_name_.get());
	    	            fmt % (lastArchiveSequenceNumber+1);
	    	            std::string filename=str(fmt);
						if (!boost::filesystem::exists(boost::filesystem::path(filename)))
							break;
	            	}
				}
				break;
	        }

            boost::format fmt(options_.base_xmltar_file_name_.get());
            fmt % lastArchiveSequenceNumber;
            std::string filename=str(fmt);

            XmltarArchive xmltarArchive(options_,filename, filesToBeArchived_);
			xmltarArchive.append(lastArchiveSequenceNumber);
		}
		else if (options_.operation_ && options_.operation_==XmltarOptions::CREATE){
			if (!options_.starting_sequence_number_){
				throw std::logic_error("XmltarRun::XmltarRun: must specify starting sequence number to create multivolume archive");
			}

            boost::format fmt(options_.base_xmltar_file_name_.get());
            fmt % options_.starting_sequence_number_.get();
            std::string filename=str(fmt);

			XmltarArchive xmltarArchive(options_,filename, filesToBeArchived_);
			xmltarArchive.create(options_.starting_sequence_number_.get());
		}
	}
	else {
        boost::format fmt(options_.base_xmltar_file_name_.get());
        fmt % options_.starting_sequence_number_.get();
        std::string filename=str(fmt);

	}
#endif
}
