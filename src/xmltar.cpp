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

#include "common/Xmltar/XmltarRun.hpp"
#include "Include/Incremental_File.hpp"
#include "Snapshot/Snapshot.hpp"

int main(int argc, char *argv[])
{
	std::string line, result;
	std::ifstream ifs("snapshot-template.xml");
	while(std::getline(ifs,line)){
		result+=line;
	}

	Snapshot snapshot("snapshot-template.xml");
#if 0
	try {
	    DEBUGCXX(debugcxx,"main");
		Xmltar xmltar(argc, argv);

		switch(xmltar.options_.operation){
			case XmltarOptions::APPEND:
			case XmltarOptions::CREATE:
				xmltar.Add_To_Archive();
				break;
			case XmltarOptions::EXTRACT:
				xmltar.Extract();
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
	catch (...){
		std::cerr << "exception thrown" << std::endl;
		exit(-1);
	}

#endif
	return 0;
}
