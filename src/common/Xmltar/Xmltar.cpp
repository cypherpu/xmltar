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

#include "Xmltar/Xmltar.hpp"

#include <limits>
#include "Tar_Options.hpp"

Xmltar::Xmltar(int argc, char *argv[])
	: version("Xmltar_0_0_1") {
	Parse_Opts::Option_Parser p;

	p.Add_Option(Parse_Opts::ARGS_0,"-c","--create","create archive",
			p.Assign_Value(this->options_.operation,XmltarOptions::CREATE));
	p.Add_Option(Parse_Opts::ARGS_0,"-r","--append","append to existing archive",
			p.Assign_Value(this->options_.operation,XmltarOptions::APPEND));
	p.Add_Option(Parse_Opts::ARGS_0,"-t","--list","list table of contents of existing archive",
			p.Assign_Value(this->options_.operation,XmltarOptions::LIST));
	p.Add_Option(Parse_Opts::ARGS_0,"-x","--extract","extract files from an archive",
			p.Assign_Value(this->options_.operation,XmltarOptions::EXTRACT));
	p.Add_Option(Parse_Opts::ARGS_0,"-v","--verbose","report more details",
			p.Increment_Value(this->options_.verbosity));

    p.Add_Option(Parse_Opts::ARGS_0,"-z","--gzip","compress archive",
            p.Assign_Value(this->options_.postcompress, XmltarOptions::GZIP));
    p.Add_Option(Parse_Opts::ARGS_0,"","--bzip2","compress archive",
            p.Assign_Value(this->options_.postcompress, XmltarOptions::BZIP2));
    p.Add_Option(Parse_Opts::ARGS_0,"","--xz","compress archive",
            p.Assign_Value(this->options_.postcompress, XmltarOptions::XZ));

    p.Add_Option(Parse_Opts::ARGS_0,"","--pre-gzip","pre-compress files before archiving",
            p.Assign_Value(this->options_.precompress, XmltarOptions::GZIP));
    p.Add_Option(Parse_Opts::ARGS_0,"","--pre-bzip2","pre-compress files before archiving",
            p.Assign_Value(this->options_.precompress, XmltarOptions::BZIP2));
    p.Add_Option(Parse_Opts::ARGS_0,"","--pre-xz","pre-compress files before archiving",
            p.Assign_Value(this->options_.precompress, XmltarOptions::XZ));

    p.Add_Option(Parse_Opts::ARGS_0,"","--base16","base16 encode files before archiving",
            p.Assign_Value(this->options_.encoding,XmltarOptions::BASE16));
    p.Add_Option(Parse_Opts::ARGS_0,"","--base64","base64 encode files before archiving",
            p.Assign_Value(this->options_.encoding,XmltarOptions::BASE64));

	p.Add_Option(Parse_Opts::ARGS_0,  "","--compress-listed-incremental","compress listed-incremental file",
			p.Assign_Value(this->options_.compress_listed_incremental_file,true));
	p.Add_Option(Parse_Opts::ARGS_1,"-f","--file","specify archive to create/append/list/extract",
			p.Assign_Args(this->options_.base_xmltar_file_name));
	p.Add_Option(Parse_Opts::ARGS_1,"","--exclude","specify PATTERN to exclude files from archiving",
			p.Append_Args(this->options_.exclude_files));
	p.Add_Option(Parse_Opts::ARGS_1,"-g","--listed-incremental","work with listed-incremental archives",
			p.Assign_Args(this->options_.listed_incremental_file),
			p.Assign_Value(this->options_.incremental,true));
	p.Add_Option(Parse_Opts::ARGS_0,"-M","--multi-volume","work with multivolume archives",
			p.Assign_Value(this->options_.multi_volume,true));
	bool tape_length_option=false;
	p.Add_Option(Parse_Opts::ARGS_1,"-L","--tape-length","specify size of a multivolume archive",
            p.Assign_Args(this->options_.tape_length),
            p.Assign_Value(tape_length_option,true));
	p.Add_Option(Parse_Opts::ARGS_1,"","--starting-volume","starting volume number of a multivolume archive",
			p.Assign_Args(this->options_.starting_sequence_number));
	p.Add_Option(Parse_Opts::ARGS_1,"-T","--files-from","specify source of files to archive",
			p.Assign_Args(this->options_.files_from));
    p.Add_Option(Parse_Opts::ARGS_1,"","--stop-after","stop archiving additional files once n volumes have been created",
            p.Assign_Args(this->options_.stop_after));
    p.Add_Option(Parse_Opts::ARGS_1,"","--tag","tag archived files with identifier",
            p.Assign_Args(this->options_.tag));
	p.Add_Option(Parse_Opts::ARGS_0,"","--no-tabs","do not tab-indent the xml archive",
			p.Assign_Value(this->options_.tabs_,false));
	p.Add_Option(Parse_Opts::ARGS_0,"","--no-newlines","do not include any newlines in the xml archive",
			p.Assign_Value(this->options_.newlines_,false));

	std::vector<std::string> remaining_args=p.Parse(argc,argv);
	for(std::vector<std::string>::iterator i=remaining_args.begin(); i!=remaining_args.end(); ++i)
		options_.source_files.push_back(boost::filesystem::path(*i));

	options_.current_sequence_number=options_.starting_sequence_number;

	if (options_.verbosity==3){
		std::cerr << "Argc=" << argc << std::endl;
		if (options_.operation==XmltarOptions::APPEND) std::cerr << "Operation=APPEND" << std::endl;
		if (options_.operation==XmltarOptions::CREATE) std::cerr << "Operation=CREATE" << std::endl;
		if (options_.operation==XmltarOptions::LIST) std::cerr << "Operation=LIST" << std::endl;
		if (options_.operation==XmltarOptions::EXTRACT) std::cerr << "Operation=EXTRACT" << std::endl;
		std::cerr << "verbosity=" << options_.verbosity << std::endl;
		if (options_.multi_volume) std::cerr << "Multivolume" << std::endl;
		std::cerr << "starting_sequence_number=" << options_.starting_sequence_number << std::endl;
		std::cerr << "listed-incremental=" << options_.incremental << std::endl;
		std::cerr << "tape length=" << options_.tape_length << std::endl;
		std::cerr << "stop after=" << options_.stop_after << std::endl;
		std::cerr << "base_xmltar_file_name=" << options_.base_xmltar_file_name << std::endl;
		std::cerr << "Source file size=" << options_.source_files.size() << std::endl;
		for(std::vector<boost::filesystem::path>::iterator i=options_.exclude_files.begin(); i!=options_.exclude_files.end(); ++i)
			std::cerr << "Exclude file=" << *i << std::endl;
		for(std::vector<boost::filesystem::path>::iterator i=options_.source_files.begin(); i!=options_.source_files.end(); ++i)
			std::cerr << "Source file=" << *i << std::endl;
		std::cerr << "listed-incremental file=" << options_.listed_incremental_file << std::endl;
		std::cerr << "files from=" << options_.files_from << std::endl;
	}

	if (tape_length_option && !options_.multi_volume)
	    throw "Xmltar::Xmltar: tape_length specification requires multivolume specification (to indicate overflow archive name)";
}
