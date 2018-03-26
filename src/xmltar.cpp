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

#include <fstream>
#include <iostream>
#include <boost/format.hpp>

#include "common/Xmltar/XmltarInvocation.hpp"
#include "Include/Incremental_File.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Options/XmltarOptions.hpp"
#include "common/Compression/Compression.hpp"

int main(int argc, char const *argv[])
{
	if (!CorrectCompressorVersion(Compression::IDENTITY))
		throw std::runtime_error("main: wrong version 'cat' command");

	if (!CorrectCompressorVersion(Compression::GZIP))
		throw std::runtime_error("main: wrong version 'gzip' command");

	if (!CorrectCompressorVersion(Compression::BZIP2))
		throw std::runtime_error("main: wrong version 'bzip2' command");

	if (!CorrectCompressorVersion(Compression::LZIP))
		throw std::runtime_error("main: wrong version 'lzip' command");

	XmltarOptions options;
	options.ProcessOptions(argc, argv);

	XmltarInvocation xmltarInvocation(options);

	std::string line, result;
	std::ifstream ifs("snapshot-template.xml");
	while(std::getline(ifs,line)){
		result+=line;
	}

	if (false)
		if (boost::filesystem::exists(xmltarInvocation.Options().listed_incremental_file_.get()))
			Snapshot snapshot(xmltarInvocation.Options().listed_incremental_file_.get().string());
		else {	// create snapshot file
			if (xmltarInvocation.Options().verbosity_>=1)
				std::cerr << "xmltar: creating snapshot file " << xmltarInvocation.Options().listed_incremental_file_.get() << std::endl;
		}

	try {
	    DEBUGCXX(debugcxx,"main");

	    if (!xmltarInvocation.Options().operation_)
	    	throw std::logic_error("xmltar: called without operation");

		switch(xmltarInvocation.Options().operation_.get()){
			case XmltarOptions::CREATE:
			case XmltarOptions::APPEND:
				//xmltar.Add_To_Archive();
				break;
			case XmltarOptions::EXTRACT:
				//xmltar.Extract();
				break;
			default:
				throw ;
				break;
		}
	}
	catch (char const *msg){
		std::cerr << msg << std::endl;
		exit(-1);
	}
	catch (char *msg){
		std::cerr << msg << std::endl;
		exit(-1);
	}
	catch (std::string & msg){
		std::cerr << msg << std::endl;
		exit(-1);
	}
	catch (std::exception & e){
		std::cerr << e.what() << std::endl;
		exit(-1);
	}
	catch (...){
		std::cerr << "exception thrown" << std::endl;
		exit(-1);
	}

	return 0;
}
