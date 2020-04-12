/*
 * XmltarOptions.cpp
 *
 *  Created on: Nov 19, 2017
 *      Author: dbetz
 */

#include <iostream>

#include "Xmltar/XmltarOptions.hpp"
#include "Generated/Utilities/GzipRaw.hpp"
#include "Generated/Utilities/Gunzip.hpp"
#include "Generated/Utilities/IdentityRaw.hpp"
#include "Generated/Utilities/Identity.hpp"
#include "Generated/Utilities/ZstdCompressRaw.hpp"
#include "Generated/Utilities/ZstdDecompress.hpp"
#include "Generated/Utilities/Crypto/EncryptXChaCha20Poly1305.hpp"
#include "Generated/Utilities/Crypto/DecryptXChaCha20Poly1305.hpp"

void XmltarOptions::ProcessOptions(int argc, char const *argv[]){
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
			p.Assign_Value(fileCompression_, (CompressorGeneralInterface *) new Compressor<Identity>()),
			p.Assign_Value(fileRawCompression_, (CompressorRawInterface *) new CompressorRaw<IdentityRaw>()),
			p.Assign_Value(fileDecompression_, (CompressorGeneralInterface *) new Compressor<Identity>())
	);
	p.Add_Option(Parse_Opts::ARGS_0,"","--file-gzip","file-compress files before archiving",
			p.Assign_Value(fileCompression_, (CompressorGeneralInterface *) new Compressor<Zlib::Gzip>()),
			p.Assign_Value(fileRawCompression_, (CompressorRawInterface *) new CompressorRaw<Zlib::GzipRaw>()),
			p.Assign_Value(fileDecompression_, (CompressorGeneralInterface *) new Compressor<Zlib::Gunzip>())
	);
	p.Add_Option(Parse_Opts::ARGS_0,"","--file-zstd","file-compress files before archiving",
			p.Assign_Value(fileCompression_, (CompressorGeneralInterface *) new Compressor<ZstdCompress>()),
			p.Assign_Value(fileRawCompression_, (CompressorRawInterface *) new CompressorRaw<ZstdCompressRaw>()),
			p.Assign_Value(fileDecompression_, (CompressorGeneralInterface *) new Compressor<ZstdDecompress>())
	);


	p.Add_Option(Parse_Opts::ARGS_0,"","--identity","compress archive",
			p.Assign_Value(archiveCompression_,(CompressorGeneralInterface *) new Compressor<Identity>()),
			p.Assign_Value(archiveRawCompression_,(CompressorRawInterface *) new CompressorRaw<IdentityRaw>()),
			p.Assign_Value(archiveDecompression_,(CompressorGeneralInterface *) new Compressor<Identity>())
	);
	p.Add_Option(Parse_Opts::ARGS_0,"-z","--gzip","compress archive",
			p.Assign_Value(archiveCompression_,(CompressorGeneralInterface *) new Compressor<Zlib::Gzip>()),
			p.Assign_Value(archiveRawCompression_,(CompressorRawInterface *) new CompressorRaw<Zlib::GzipRaw>()),
			p.Assign_Value(archiveDecompression_,(CompressorGeneralInterface *) new Compressor<Zlib::Gunzip>())
	);
	p.Add_Option(Parse_Opts::ARGS_0,"","--zstd","compress archive",
			p.Assign_Value(archiveCompression_, (CompressorGeneralInterface *) new Compressor<ZstdCompress>()),
			p.Assign_Value(archiveRawCompression_, (CompressorRawInterface *) new CompressorRaw<ZstdCompressRaw>()),
			p.Assign_Value(archiveDecompression_, (CompressorGeneralInterface *) new Compressor<ZstdDecompress>())
	);

	p.Add_Option(Parse_Opts::ARGS_0,"","--encrypt","encrypt/decrypt archive",
			p.Assign_Value(encrypted_,true)
	);

	p.Add_Option(Parse_Opts::ARGS_1,"-g","--listed-incremental","work with listed-incremental archives",
			p.Assign_Args(listed_incremental_file_));
	p.Add_Option(Parse_Opts::ARGS_0,  "","--listed-incremental-identity","plaintext listed-incremental file",
			p.Assign_Value(incrementalFileCompression_,(CompressorGeneralInterface *) new Compressor<Identity>()),
			p.Assign_Value(incrementalFileDecompression_,(CompressorGeneralInterface *) new Compressor<Identity>())
	);
	p.Add_Option(Parse_Opts::ARGS_0,  "","--listed-incremental-gzip","gzip listed-incremental file",
			p.Assign_Value(incrementalFileCompression_,(CompressorGeneralInterface *) new Compressor<Zlib::Gzip>()),
			p.Assign_Value(incrementalFileDecompression_,(CompressorGeneralInterface *) new Compressor<Zlib::Gunzip>())
	);
	p.Add_Option(Parse_Opts::ARGS_0,  "","--listed-incremental-zstd","zstd listed-incremental file",
			p.Assign_Value(incrementalFileCompression_,(CompressorGeneralInterface *) new Compressor<ZstdCompress>()),
			p.Assign_Value(incrementalFileDecompression_,(CompressorGeneralInterface *) new Compressor<ZstdDecompress>())
	);

	p.Add_Option(Parse_Opts::ARGS_1,"","--level","set dump level",
			p.Assign_Args(dump_level_));

	p.Add_Option(Parse_Opts::ARGS_1,"-f","--file","specify archive to create/append/list/extract",
			p.Assign_Args(base_xmltar_file_name_));
	p.Add_Option(Parse_Opts::ARGS_1,"","--exclude","specify PATTERN to exclude files from archiving",
			p.Append_Args(excludeFileGlobs_));
	p.Add_Option(Parse_Opts::ARGS_0,"-M","--multi-volume","work with multivolume archives",
			p.Assign_Value(multi_volume_,true));
	p.Add_Option(Parse_Opts::ARGS_1,"-L","--tape-length","specify size of a multivolume archive",
			p.Assign_Args(tape_length_),
			p.Assign_Args(preencryptedTapeLength_),
			p.Assign_Value(multi_volume_,true));
	p.Add_Option(Parse_Opts::ARGS_1,"","--starting-volume","starting volume number of a multivolume archive",
			p.Assign_Args(starting_volume_));
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
	p.Add_Option(Parse_Opts::ARGS_0,"","--sha3-512","compute sha3 512 and se for file comparisons",
			p.Assign_Value(newlines_,true));

	std::vector<std::string> remaining_args=p.Parse(argc,argv);

	for(std::vector<std::string>::iterator i=remaining_args.begin(); i!=remaining_args.end(); ++i){
		sourceFileGlobs_.push_back(*i);
	}

	for(std::vector<std::string>::iterator i=excludeFileGlobs_.begin(); i!=excludeFileGlobs_.end(); ++i){
		std::cerr << "ExcludeFileGlobs=" << *i << std::endl;
	}

	if (listed_incremental_file_ && !dump_level_)
		throw std::logic_error("XmltarOptions::ProcessOptions: must specify dump level for listed-incremental");
}

std::string XmltarOptions::toXMLString(){
	std::ostringstream oss;

	oss << Tabs("\t\t") << "<options>" << std::endl;

	if (operation_){
		oss << Tabs("\t\t\t") << "<option>";
		if (operation_==APPEND) oss << "--apppend";
		else if (operation_==CREATE) oss << "--create";
		else if (operation_==LIST) oss << "--list";
		else if (operation_==EXTRACT) oss << "--extract";
		else throw(std::logic_error("XmltarOptions::toXMLString: unknown operation"));
		oss << "</option>" << std::endl;
	}
	if (verbosity_)
		for(int i=0; i<verbosity_.get(); ++i)
			oss << Tabs("\t\t\t") << "<option>--verbosity</option>" << std::endl;

	if(multi_volume_)
		oss << Tabs("\t\t\t") << "<option>--multi-volume</option>" << std::endl;

	oss << Tabs("\t\t\t") << "<option>--file-" << fileCompression_.get()->CompressorName() << "</option>" << std::endl;
	oss << Tabs("\t\t\t") << "<option>--" << encoding_.get()->CompressorName() << "</option>" << std::endl;
	oss << Tabs("\t\t\t") << "<option>--" << archiveCompression_.get()->CompressorName() << "</option>" << std::endl;

	if (tape_length_)
		oss << Tabs("\t\t\t") << "<option>--tape-length=" << tape_length_.get() << "</option>" << std::endl;

	if (stop_after_)
		oss << Tabs("\t\t\t") << "<option>--stop-after=" << stop_after_.get() << "</option>" << std::endl;

	if (listed_incremental_file_)
		oss << Tabs("\t\t\t") << "<option>--listed-incremental=" << listed_incremental_file_.get().string() << "</option>" << std::endl;

	if (incrementalFileCompression_->CompressorName()!=std::string("identity"))
		oss << Tabs("\t\t\t") << "<option>--compress-listed-incremental</option>" << std::endl;

	if (files_from_)
		oss << Tabs("\t\t\t") << "<option>--files-from=" << files_from_.get().string() << "</option>" << std::endl;

	if (excludeFileGlobs_.size())
		for(auto & i : excludeFileGlobs_)
			oss << Tabs("\t\t\t") << "<option>--exclude-files=" << i << "</option>" << std::endl;

	if (archiveMemberTag_)
		oss << Tabs("\t\t\t") << "<option>--archive-member-tag=" << archiveMemberTag_.get() << "</option>" << std::endl;

	if (tabs_ && tabs_.get()==false)
		oss << Tabs("\t\t\t") << "<option>--no-tabs</option>" << std::endl;

	if (newlines_ && newlines_.get()==false)
		oss << Tabs("\t\t\t") << "<option>--no-newlines</option>" << std::endl;

	if (base_xmltar_file_name_)
		oss << Tabs("\t\t\t") << "<option>--file=" << base_xmltar_file_name_.get() << "</option>" << std::endl;

	if (starting_volume_)
		oss << Tabs("\t\t\t") << "<option>--starting-volume=" << starting_volume_.get() << "</option>" << std::endl;

	if (sourceFileGlobs_.size())
		for(auto & i : sourceFileGlobs_)
			oss << Tabs("\t\t\t") << "<option>" << i << "</option>" << std::endl;

	oss << Tabs("\t\t") << "</options>" << std::endl;

	return oss.str();
}
