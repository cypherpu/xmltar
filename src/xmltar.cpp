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

/*
 * ../xmltar/bazel-bin/xmltar -c -f test.id.16.gz.mv.xmltar.%04d.gz --multi-volume --starting-volume=1 --tape-length=1000000 --base16 --gzip /home/dbetz/git/xmltar
 */

#include <fstream>
#include <iostream>

#include <string>
#include <termios.h>
#include <unistd.h>

#include <boost/format.hpp>
#include <spdlog/spdlog.h>

#include "common/Xmltar/XmltarInvocation.hpp"
#include "Include/Incremental_File.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Options/XmltarOptions.hpp"

#include "Transform/TransformIdentity.hpp"
#include "Transform/TransformGzip.hpp"
#include "Transform/TransformBzip2.hpp"
#include "Transform/TransformLzip.hpp"

int main(int argc, char const *argv[])
{
	spdlog::set_level(spdlog::level::info);
	spdlog::set_pattern("[%Y-%m-%d] [%H:%M:%S %z] [%l] [%P] %v");

	try {
		/*
		TransformIdentity transformIdentity;
		TransformGzip transformGzip;
		TransformBzip2 transformBzip2;
		TransformLzip transformLzip;

		if (!transformIdentity.CorrectCompressorVersion())
			throw std::runtime_error("main: wrong version 'cat' command");

		if (!transformGzip.CorrectCompressorVersion())
			throw std::runtime_error("main: wrong version 'gzip' command");

		if (!transformBzip2.CorrectCompressorVersion())
			throw std::runtime_error("main: wrong version 'bzip2' command");

		if (!transformLzip.CorrectCompressorVersion())
			throw std::runtime_error("main: wrong version 'lzip' command");
		*/

	    termios oldt;
	    tcgetattr(STDIN_FILENO, &oldt);
	    termios newt = oldt;
	    newt.c_lflag &= ~ECHO;
	    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	    std::string s;
	    std::cout << "Before getline" << std::endl;
	    std::getline(std::cin, s);
	    std::cout << "After getline" << std::endl;

	    std::cout << s << std::endl;

	    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);


		XmltarOptions options;
		options.ProcessOptions(argc, argv);

		XmltarInvocation xmltarInvocation(options);
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


#if 0
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
#endif

	return 0;
}
