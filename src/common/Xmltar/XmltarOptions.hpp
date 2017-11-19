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

#include "Compression/Compression.hpp"
#include "Options/Options-TarStyle.hpp"

class XmltarOptions {
public:
    enum Operation { NOOP, APPEND, CREATE, LIST, EXTRACT };
	enum Encoding { BASE16, BASE64 };

	Operation operation_;
	int verbosity_;
	bool multi_volume_;
	Compression fileCompress_;
	Encoding encoding_;
	Compression archiveMemberCompress_;
	Compression archiveCompress_;
	bool incremental_;
	bool compress_listed_incremental_file_;
	size_t tape_length_;
	size_t stop_after_;
	std::vector<boost::filesystem::path> source_files_;
	boost::filesystem::path listed_incremental_file_;
	boost::filesystem::path files_from_;
	std::vector<boost::filesystem::path> exclude_files_;
	std::string archiveMemberTag_;
   	bool tabs_;
   	bool newlines_;

	std::string base_xmltar_file_name_;							// xmltar_file;
	unsigned int starting_sequence_number_;						// starting_volume;

	std::string current_xmltar_file_name_;
	unsigned int current_sequence_number_;

    XmltarOptions(void)
        : operation_(NOOP), verbosity_(0), multi_volume_(false), fileCompress_(IDENTITY), archiveMemberCompress_(IDENTITY), archiveCompress_(IDENTITY),
          encoding_(BASE16), incremental_(false), compress_listed_incremental_file_(false),
          tape_length_(std::numeric_limits<std::size_t>::max()), stop_after_(std::numeric_limits<std::size_t>::max()),
		  archiveMemberTag_(""), tabs_(true), newlines_(true), starting_sequence_number_(0), current_sequence_number_(0) { }

    void ProcessOptions(int argc, char const *argv[]){
    	std::cerr << "ProcessOptions: " << 0 << std::endl;

    	Parse_Opts::Option_Parser p;

    	p.Add_Option(Parse_Opts::ARGS_0,"-c","--create","create archive",
    			p.Assign_Value(operation_,XmltarOptions::CREATE));
    	p.Add_Option(Parse_Opts::ARGS_0,"-r","--append","append to existing archive",
    			p.Assign_Value(operation_,XmltarOptions::APPEND));
    	p.Add_Option(Parse_Opts::ARGS_0,"-t","--list","list table of contents of existing archive",
    			p.Assign_Value(operation_,XmltarOptions::LIST));
    	p.Add_Option(Parse_Opts::ARGS_0,"-x","--extract","extract files from an archive",
    			p.Assign_Value(operation_,XmltarOptions::EXTRACT));
    	p.Add_Option(Parse_Opts::ARGS_0,"-v","--verbose","report more details",
    			p.Increment_Value(verbosity_));

        p.Add_Option(Parse_Opts::ARGS_0,"","--file-gzip","file-compress files before archiving",
                p.Assign_Value(fileCompress_, Compression::GZIP));
        p.Add_Option(Parse_Opts::ARGS_0,"","--file-bzip2","file-compress files before archiving",
                p.Assign_Value(fileCompress_, Compression::BZIP2));
        p.Add_Option(Parse_Opts::ARGS_0,"","--file-lzip","file-compress files before archiving",
                p.Assign_Value(fileCompress_, Compression::LZIP));

        p.Add_Option(Parse_Opts::ARGS_0,"","--base16","base16 encode files before archiving",
                p.Assign_Value(encoding_,XmltarOptions::BASE16));
        p.Add_Option(Parse_Opts::ARGS_0,"","--base64","base64 encode files before archiving",
                p.Assign_Value(encoding_,XmltarOptions::BASE64));

        p.Add_Option(Parse_Opts::ARGS_0,"","--file-gzip","member-compress members before archiving",
                p.Assign_Value(fileCompress_, Compression::GZIP));
        p.Add_Option(Parse_Opts::ARGS_0,"","--file-bzip2","member-compress members before archiving",
                p.Assign_Value(fileCompress_, Compression::BZIP2));
        p.Add_Option(Parse_Opts::ARGS_0,"","--file-lzip","member-compress members before archiving",
                p.Assign_Value(fileCompress_, Compression::LZIP));

        p.Add_Option(Parse_Opts::ARGS_0,"-z","--gzip","compress archive",
                p.Assign_Value(archiveCompress_, Compression::GZIP));
        p.Add_Option(Parse_Opts::ARGS_0,"","--bzip2","compress archive",
                p.Assign_Value(archiveCompress_, Compression::BZIP2));
        p.Add_Option(Parse_Opts::ARGS_0,"","--lzip","compress archive",
                p.Assign_Value(archiveCompress_, Compression::LZIP));

    	p.Add_Option(Parse_Opts::ARGS_0,  "","--compress-listed-incremental","compress listed-incremental file",
    			p.Assign_Value(compress_listed_incremental_file_,true));
    	p.Add_Option(Parse_Opts::ARGS_1,"-g","--listed-incremental","work with listed-incremental archives",
    			p.Assign_Args(listed_incremental_file_),
    			p.Assign_Value(incremental_,true));


    	p.Add_Option(Parse_Opts::ARGS_1,"-f","--file","specify archive to create/append/list/extract",
    			p.Assign_Args(base_xmltar_file_name_));
    	p.Add_Option(Parse_Opts::ARGS_1,"","--exclude","specify PATTERN to exclude files from archiving",
    			p.Append_Args(exclude_files_));
    	p.Add_Option(Parse_Opts::ARGS_0,"-M","--multi-volume","work with multivolume archives",
    			p.Assign_Value(multi_volume_,true));
    	bool tape_length_option=false;
    	p.Add_Option(Parse_Opts::ARGS_1,"-L","--tape-length","specify size of a multivolume archive",
                p.Assign_Args(tape_length_),
    			p.Assign_Value(multi_volume_,true),
                p.Assign_Value(tape_length_option,true));
    	p.Add_Option(Parse_Opts::ARGS_1,"","--starting-volume","starting volume number of a multivolume archive",
    			p.Assign_Args(starting_sequence_number_));
    	p.Add_Option(Parse_Opts::ARGS_1,"-T","--files-from","specify source of files to archive",
    			p.Assign_Args(files_from_));
        p.Add_Option(Parse_Opts::ARGS_1,"","--stop-after","stop archiving additional files once n volumes have been created",
                p.Assign_Args(stop_after_));
        p.Add_Option(Parse_Opts::ARGS_1,"","--tag","tag archived files with identifier",
                p.Assign_Args(archiveMemberTag_));
    	p.Add_Option(Parse_Opts::ARGS_0,"","--no-tabs","do not tab-indent the xml archive",
    			p.Assign_Value(tabs_,false));
    	p.Add_Option(Parse_Opts::ARGS_0,"","--no-newlines","do not include any newlines in the xml archive",
    			p.Assign_Value(newlines_,false));

    	std::vector<std::string> remaining_args=p.Parse(argc,argv);
    	for(std::vector<std::string>::iterator i=remaining_args.begin(); i!=remaining_args.end(); ++i)
    		source_files_.push_back(boost::filesystem::path(*i));

    	current_sequence_number_=starting_sequence_number_;
   }

	std::string Tabs(const char *tabStr){
		return tabs_?std::string(tabStr):std::string("");
	}

	std::string Newline(){
		return newlines_?std::string("\n"):std::string("");
	}
};

#endif // XMLTAROPTIONS_HPP_
