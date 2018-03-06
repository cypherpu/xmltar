/*
 * ToDecimalInt.cpp
 *
 *  Created on: Feb 24, 2018
 *      Author: dbetz
 */

#include <sstream>

#include "Utilities/ToDecimalInt.hpp"

std::string ToDecimalInt(int i){
	std::ostringstream oss;

	oss << std::dec << i;

	return oss.str();
}
