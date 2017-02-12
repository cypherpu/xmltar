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

#if 0

#include <stdlib.h>
#include "Debug.hpp"

int Debug::depth=0;

void Terminate(std::ostream& os){
    os << "Terminated" << std::endl;
    exit(-1);
}

void Terminate(const char *msg){
    std::cerr << msg << std::endl;
    std::cerr << "Terminated" << std::endl;
    exit(-1);
}

#endif
