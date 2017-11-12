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

#ifndef DEBUG_HPP_
#define DEBUG_HPP_

#include <iostream>

#if 0

class Debug {
	const char *module_name_;
	static int depth;
public:
	Debug(void){
		throw "Debug::Debug: called without module name argument";
	}
	Debug(const char *module_name)
		: module_name_(module_name) {
	    for(size_t i=0; i<depth; ++i)
	        std::cerr << "    ";
		std::cerr << module_name_ << ": entering" << std::endl;
		++depth;
	}
	~Debug(){
        --depth;
        for(size_t i=0; i<depth; ++i)
            std::cerr << "    ";
		std::cerr << module_name_ << ": leaving" << std::endl;
	}
	std::string Tab(void){
	    return std::string(4*depth,' ');
	}
};

void Terminate(std::ostream&);
void Terminate(const char *);

#define DEBUGCXX(x,y)	Debug x(y);
#define DEBUGCXXTAB(x)  x.Tab()

#else
#define DEBUGCXX(x,y)	;
#define DEBUGCXXTAB(x)  ""
//#define Terminate(x)    throw x			// FIXME
#endif

#endif /* DEBUG_HPP_ */
