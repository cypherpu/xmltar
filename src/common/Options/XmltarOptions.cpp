/*
 * XmltarOptions.cpp
 *
 *  Created on: Nov 19, 2017
 *      Author: dbetz
 */

#include <iostream>

#include "Options/XmltarOptions.hpp"

void XmltarOptions::ProcessOptions(int argc, char const *argv[]){
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

	p.Add_Option(Parse_Opts::ARGS_0,"","--file-identity","file-compress files before archiving",
			p.Assign_Value(fileCompress_, Compression::IDENTITY));
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

	p.Add_Option(Parse_Opts::ARGS_0,"","--member-gzip","member-compress members before archiving",
			p.Assign_Value(archiveMemberCompress_, Compression::GZIP));
	p.Add_Option(Parse_Opts::ARGS_0,"","--member-bzip2","member-compress members before archiving",
			p.Assign_Value(archiveMemberCompress_, Compression::BZIP2));
	p.Add_Option(Parse_Opts::ARGS_0,"","--member-lzip","member-compress members before archiving",
			p.Assign_Value(archiveMemberCompress_, Compression::LZIP));

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

	for(std::vector<std::string>::iterator i=remaining_args.begin(); i!=remaining_args.end(); ++i){
		if (!source_files_)
			source_files_=std::vector<boost::filesystem::path>();
		source_files_.get().push_back(boost::filesystem::path(*i));
	}

	current_sequence_number_=starting_sequence_number_.get();
}

std::string XmltarOptions::toXMLString(){
	std::ostringstream oss;

	oss << Tabs("\t\t") << "<options>" << std::endl;

	oss << Tabs("\t\t\t") << "<operation>" << std::endl;
	if (operation_==APPEND) oss << "--apppend";
	else if (operation_==CREATE) oss << "--create";
	else if (operation_==LIST) oss << "--list";
	else if (operation_==EXTRACT) oss << "--extract";
	else oss << "UNKNOWN OPERATION";
	oss << "</operation>" << std::endl;

	for(int i=0; i<verbosity_; ++i)
		oss << Tabs("\t\t\t") << "<verbosity>-v</verbosity>" << std::endl;

	if(multi_volume_)
		oss << Tabs("\t\t\t") << "<multi-volume>--multi-volume</verbosity>" << std::endl;

	oss << Tabs("\t\t\t") << "<file-compression>" << std::endl;
	if (fileCompress_==IDENTITY) oss << "--file-identity";
	else if (fileCompress_==GZIP) oss << "--file-gzip";
	else if (fileCompress_==BZIP2) oss << "--file-bzip2";
	else if (fileCompress_==LZIP) oss << "--file-lzip";
	else oss << "UNKNOWN OPERATION";
	oss << "</file-compression>" << std::endl;

	oss << Tabs("\t\t\t") << "<file-encoding>" << std::endl;
	if (encoding_==BASE16) oss << "--base16";
	else if (encoding_==BASE64) oss << "--base64";
	else oss << "UNKNOWN OPERATION";
	oss << "</file-encoding>" << std::endl;

	oss << Tabs("\t\t\t") << "<member-compression>" << std::endl;
	if (archiveMemberCompress_==IDENTITY) oss << "--member-identity";
	else if (archiveMemberCompress_==GZIP) oss << "--member-gzip";
	else if (archiveMemberCompress_==BZIP2) oss << "--member-bzip2";
	else if (archiveMemberCompress_==LZIP) oss << "--member-lzip";
	else oss << "UNKNOWN OPERATION";
	oss << "</member-compression>" << std::endl;

	oss << Tabs("\t\t\t") << "<archive-compression>" << std::endl;
	if (archiveCompress_==IDENTITY) oss << "--archive-identity";
	else if (archiveCompress_==GZIP) oss << "--archive-gzip";
	else if (archiveCompress_==BZIP2) oss << "--archive-bzip2";
	else if (archiveCompress_==LZIP) oss << "--archive-lzip";
	else oss << "UNKNOWN OPERATION";
	oss << "</archive-compression>" << std::endl;

	if (incremental_)
		oss << Tabs("\t\t\t") << "<incremental>--incremental</incremental>" << std::endl;

	if (compress_listed_incremental_file_)
		oss << Tabs("\t\t\t") << "<compress-listed-incremental>--compress-listed-incremental</compress-listed-incremental>" << std::endl;

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



	return oss.str();
}
