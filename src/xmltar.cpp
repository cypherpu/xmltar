/*

xmltar.cpp
Copyright 2010-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: 2010
Author: dbetz

This file is part of Xmltar.

Xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Xmltar.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <fstream>
#include <iostream>

#include <string>
#include <termios.h>
#include <unistd.h>

#include <json.hpp>

#include <boost/format.hpp>
#include <spdlog/spdlog.h>

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>

#include "Xmltar/XmltarInvocation.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Xmltar/XmltarMemberCreate.hpp"
#include "Xmltar/XmltarGlobals.hpp"

#include "Utilities/FromHex.hpp"
#include "Utilities/Crypto/GenerateKey.hpp"

/*

/home/dbetz/git/xmltar/bazel-bin/xmltar --create --file /backup/bluray_mnt/backup_2020_09_20.xmltar --file-zstd --zstd --encrypt /home/dbetz
/home/dbetz/git/xmltar/bazel-bin/xmltar --create --file /backup/bluray_mnt/backup_2020_09_20.%03d.xmltar --multi-volume --tape-length 1000000000 --starting-volume 1 --file-zstd --zstd --encrypt --read-fifo /backup/xmltar_read --write-fifo /backup/xmltar_write /home/dbetz/Music/FLAC/

 */
int main(int argc, char const *argv[])
{
	spdlog::set_level(spdlog::level::info);
	spdlog::set_pattern("[%Y-%m-%d] [%H:%M:%S %z] [%l] [%P] %v");

	XmltarGlobals globals;

	try {
		std::ifstream ifs("/home/dbetz/git/Private/xmltar.json");
		auto j=nlohmann::json::parse(ifs);
		globals.passphrase_=j["passphrase"];
		globals.salt_=FromEscapedHex(j["salt"]);

	    globals.key_=GenerateKey(globals.passphrase_,globals.salt_);

		globals.options_.ProcessOptions(argc, argv);
		XmltarInvocation xmltarInvocation(globals);
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
		if (std::filesystem::exists(xmltarInvocation.Options().listed_incremental_file_.get()))
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

	for(auto i : globals.errorMessages_)
		std::cerr << i << std::endl;

	return globals.resultCode_;;
}
