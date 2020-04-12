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

#include <glob.h>

#include <limits>
#include <stdexcept>
#include <filesystem>
#include <set>

#include <boost/format.hpp>
#include <boost/random.hpp>

#include <spdlog/spdlog.h>

#include "Utilities/Glob.hpp"
#include "Utilities/Options-TarStyle.hpp"
#include "Utilities/Crypto/EncryptXChaCha20Poly1305.hpp"
#include "Utilities/Crypto/DecryptXChaCha20Poly1305.hpp"
#include "Utilities/Options-TarStyle.hpp"
#include "Snapshot/Snapshot.hpp"

#include "Xmltar/XmltarInvocation.hpp"
#include "Xmltar/XmltarMemberCreate.hpp"
#include "Xmltar/XmltarArchiveCreateSingleVolume.hpp"
#include "Xmltar/XmltarArchiveCreateMultiVolume.hpp"
#include "Xmltar/XmltarArchiveExtractMultiVolume.hpp"
#include "Xmltar/XmltarArchiveExtractSingleVolume.hpp"

static int globError=0;
static char const *globErrorPath=nullptr;

extern "C" int GlobErrorFunction(char const *epath, int eerrno){
	globErrorPath=epath;
	globError=eerrno;

	return 1;
}

XmltarInvocation::XmltarInvocation(XmltarGlobals & globals)
	: version("Xmltar_0_0_1"), globals_(globals)  {

	if (globals_.options_.verbosity_==3){
		if (globals_.options_.operation_==XmltarOptions::APPEND) std::cerr << "Operation=APPEND" << std::endl;
		if (globals_.options_.operation_==XmltarOptions::CREATE) std::cerr << "Operation=CREATE" << std::endl;
		if (globals_.options_.operation_==XmltarOptions::LIST) std::cerr << "Operation=LIST" << std::endl;
		if (globals_.options_.operation_==XmltarOptions::EXTRACT) std::cerr << "Operation=EXTRACT" << std::endl;
		std::cerr << "verbosity=" << globals_.options_.verbosity_.get() << std::endl;
		if (globals_.options_.multi_volume_) std::cerr << "Multivolume" << std::endl;
		if (globals_.options_.starting_volume_) std::cerr << "starting_volume=" << globals_.options_.starting_volume_.get() << std::endl;
		if (globals_.options_.preencryptedTapeLength_) std::cerr << "tape length=" << globals_.options_.preencryptedTapeLength_.get() << std::endl;
		if (globals_.options_.stop_after_) std::cerr << "stop after=" << globals_.options_.stop_after_.get() << std::endl;
		std::cerr << "base_xmltar_file_name=" << globals_.options_.base_xmltar_file_name_.get() << std::endl;
		if (globals_.options_.sourceFileGlobs_.size()) std::cerr << "Source file size=" << globals_.options_.sourceFileGlobs_.size() << std::endl;
		if (globals_.options_.excludeFileGlobs_.size())
			for(auto & i : globals_.options_.excludeFileGlobs_)
				std::cerr << "Exclude file=" << i << std::endl;
		if (globals_.options_.sourceFileGlobs_.size())
			for(auto & i : globals_.options_.sourceFileGlobs_)
				std::cerr << "Source file=" << i << std::endl;
		if (globals_.options_.listed_incremental_file_) std::cerr << "listed-incremental file=" << globals_.options_.listed_incremental_file_.get() << std::endl;
		if (globals_.options_.files_from_) std::cerr << "files from=" << globals_.options_.files_from_.get() << std::endl;
	}

	if (globals_.options_.encrypted_){
		globals_.options_.archiveEncryption_.reset(new EncryptXChaCha20Poly1305Decorator(globals_.xChaCha20Poly1305MessageLength));
		globals_.options_.archiveDecryption_.reset(new DecryptXChaCha20Poly1305Decorator(globals_.xChaCha20Poly1305MessageLength));
		globals_.options_.snapshotEncryption_.reset(new EncryptXChaCha20Poly1305Decorator(globals_.xChaCha20Poly1305MessageLength));
		globals_.options_.snapshotDecryption_.reset(new DecryptXChaCha20Poly1305Decorator(globals_.xChaCha20Poly1305MessageLength));

		if (globals_.options_.multi_volume_){
			if (!globals_.options_.tape_length_)
				throw std::runtime_error("XmltarInvocation::XmltarInvocation: when multi-volume is used, tape_length must be specified");
			size_t nMessages=(globals_.options_.tape_length_.get()-crypto_secretstream_xchacha20poly1305_HEADERBYTES-crypto_secretstream_xchacha20poly1305_ABYTES)
									/(globals_.xChaCha20Poly1305MessageLength+crypto_secretstream_xchacha20poly1305_ABYTES);
			size_t remainder=(globals_.options_.tape_length_.get()-crypto_secretstream_xchacha20poly1305_HEADERBYTES-crypto_secretstream_xchacha20poly1305_ABYTES)
									%(globals_.xChaCha20Poly1305MessageLength+crypto_secretstream_xchacha20poly1305_ABYTES);
			size_t compressedTextLength=nMessages*globals_.xChaCha20Poly1305MessageLength+remainder;

			globals_.options_.preencryptedTapeLength_=nMessages*globals_.xChaCha20Poly1305MessageLength+remainder;
		}
	}

	spdlog::debug("Before if (options_.sourceFileGlobs_.size())");
	{
		 if (globals_.options_.files_from_){
			std::ifstream ifs(globals_.options_.files_from_.get().string());

			if (ifs){
				std::string line;
				while(std::getline(ifs,line))
					globals_.options_.sourceFileGlobs_.push_back(line);
			}
			else
				throw std::runtime_error("XmltarInvocation::XmltarInvocation: cannot open files_from");
		}

		if (globals_.options_.sourceFileGlobs_.size()==0)
			if (globals_.options_.operation_==XmltarOptions::CREATE || globals_.options_.operation_==XmltarOptions::APPEND)
				throw std::runtime_error("XmltarInvocation::XmltarInvocation: no files specified");

		MatchingPathsFromGlobs(globals_.options_.sourceFileGlobs_,&globals_.filesToBeIncluded_);
		globals_.filesToBeIncluded_.push(ExtendedPath(ExtendedPath::PathType::MAX));
	}

	spdlog::debug("Before if (options_.excludeFileGlobs_.size())");
	{
		MatchingPathsFromGlobs(globals_.options_.excludeFileGlobs_,&globals_.filesToBeExcluded_);
		globals_.filesToBeExcluded_.push(ExtendedPath(ExtendedPath::PathType::MAX));
	}

	spdlog::debug("Before boost::optional<Snapshot> snapshot(options_)");
	if (globals_.options_.listed_incremental_file_){
		if (!(globals_.options_.operation_ && globals_.options_.operation_==XmltarOptions::CREATE))
			throw std::runtime_error("XmltarInvocation: must use incremental file with create");

		globals_.snapshot_.reset(new Snapshot(globals_));
	}

	if (globals_.options_.operation_ && globals_.options_.operation_==XmltarOptions::CREATE){
		if (globals_.options_.multi_volume_){
			if (!globals_.options_.starting_volume_)
				throw std::logic_error("XmltarRun::XmltarRun: must specify starting volume number to create multivolume archive");

            globals_.current_volume_=globals_.options_.starting_volume_.get();
            globals_.NextMember();

            for(unsigned int i=0; i<globals_.options_.stop_after_.get(); ++i, ++globals_.current_volume_){
                boost::format fmt(globals_.options_.base_xmltar_file_name_.get());
                fmt % globals_.current_volume_;
                std::string filename=str(fmt);

                std::cerr << "*********" << globals_.current_volume_ << "******** " << (globals_.nextMember_ && globals_.nextMember_->Ifs()?(std::streamoff)(globals_.nextMember_->Ifs()->tellg()):(std::streamoff)0) << std::endl;
                XmltarArchiveCreateMultiVolume xmltarArchiveCreateMultiVolume(globals_,filename);
                std::cerr << "*********" << globals_.current_volume_ << "******** " << (globals_.nextMember_ && globals_.nextMember_->Ifs()?(std::streamoff)(globals_.nextMember_->Ifs()->tellg()):(std::streamoff)0) << std::endl;
                // We return from XmltarArchive under 2 circumstances:
                // 1. we ran out of files to archive
                // 2. we ran out of space in the archive

            	if (!globals_.nextMember_ && globals_.filesToBeIncluded_.top().pathType()==ExtendedPath::PathType::MAX) break;
            }
		}
		else {	// !options_.multi_volume_
			if (!globals_.options_.base_xmltar_file_name_)
				throw std::runtime_error("xmltar: XmltarInvocation: must specify an output file");

            std::unique_ptr<XmltarMemberCreate> nextMember;
            XmltarArchiveCreateSingleVolume xmltarArchiveCreateSingleVolume(globals_,globals_.options_.base_xmltar_file_name_.get(), 0);
		}
	}
	else if (globals_.options_.operation_ && globals_.options_.operation_==XmltarOptions::APPEND){
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
	if (globals_.options_.operation_ && globals_.options_.operation_==XmltarOptions::EXTRACT){
		if (globals_.options_.multi_volume_){
			if (!globals_.options_.starting_volume_)
				throw std::logic_error("XmltarRun::XmltarRun: must specify starting sequence number to create multivolume archive");

			globals_.current_volume_=globals_.options_.starting_volume_.get();
			std::shared_ptr<XmltarMemberCreate> nextMember;

			for(unsigned int i=0; i<globals_.options_.stop_after_.get(); ++i, ++globals_.current_volume_){
				boost::format fmt(globals_.options_.base_xmltar_file_name_.get());
				fmt % globals_.current_volume_;
				std::string filename=str(fmt);

				if (!std::filesystem::exists(filename)) break;
				XmltarArchiveExtractMultiVolume xmltarArchiveExtractMultiVolume(globals_,filename);
			}
		}
		else {
			std::shared_ptr<XmltarMemberCreate> nextMember;
			XmltarArchiveExtractSingleVolume xmltarArchiveSingleMultiVolume(globals_,globals_.options_.base_xmltar_file_name_.get());
		}
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
	            if (std::filesystem::exists(std::filesystem::path(filename))){
	            	for( ; ; ++lastArchiveSequenceNumber){
	    	            boost::format fmt(options_.base_xmltar_file_name_.get());
	    	            fmt % (lastArchiveSequenceNumber+1);
	    	            std::string filename=str(fmt);
						if (!std::filesystem::exists(std::filesystem::path(filename)))
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

void XmltarInvocation::MatchingPathsFromGlobs(
		std::vector<std::string> const & patterns,
		std::priority_queue<ExtendedPath,std::vector<ExtendedPath>,std::greater<ExtendedPath>> *matchingPaths
	){
	std::set<std::filesystem::path> tmp;
	for(auto & pattern : patterns){
		glob_t globs;
		globs.gl_offs=0;
		if (glob(pattern.c_str(),GLOB_ERR,GlobErrorFunction,&globs)!=0){
			std::cerr << "MatchingPathsFromGlobs: " << globErrorPath << std::endl;
			throw std::runtime_error("MatchingPathsFromGlobs: error");
		}

		for(size_t i=0; i<globs.gl_pathc; ++i)
			tmp.insert(globs.gl_pathv[i]);

		globfree(&globs);

		for(auto & i : tmp)
			matchingPaths->push(i);
	}
}

