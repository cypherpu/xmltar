/*
 * ToOctalInt.cpp
 *
 *  Created on: Feb 24, 2018
 *      Author: dbetz
 */

#include "Utilities/ToOctalInt.hpp"

#include <sstream>

std::string ToOctalInt(int i){
	std::ostringstream oss;

	oss << "0" << std::oct << i;

	return oss.str();
}
