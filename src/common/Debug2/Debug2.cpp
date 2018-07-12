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

#include "../Debug2/Debug2.hpp"

#include <iostream>


namespace betz {

int Debug2::depth=0;
int Debug2::spaces=4;

Debug2::Debug2(char const *msg)
	: msg_(msg){
	std::cerr << std::string(depth*spaces,' ') << msg_ << ": entering" << std::endl;
	depth++;
}

Debug2::~Debug2(){
	depth--;
	std::cerr << std::string(depth*spaces,' ') << msg_ << ": leaving" << std::endl;
}

std::ostream & operator<<(std::ostream & os, Debug2 const & dbg){
	os << std::string(Debug2::depth*Debug2::spaces,' ') << dbg.msg_;

	return os;
}

}	//betz
