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

#include "XmltarRun.hpp"

bool XmltarRun::Exclude_File(std::string filename){
    for(std::vector<boost::filesystem::path>::iterator i=options_.exclude_files_.begin(); i!=options_.exclude_files_.end(); ++i){
        std::cerr << filename << "==" << *i << " " << std::boolalpha << (filename==*i) << std::endl;
        if (filename==*i){
            std::cerr << "Excluding file=" << filename << std::endl;
            return true;
        }
    }

    return false;
}
