/*
 * ToHexDigit.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dbetz
 */

#include <stdexcept>

char ToHexDigit(int i){
	if (i>=0 && i<=9)
		return i+'0';
	else if (i>=10 && i<=15)
		return (i-10)+'a';
	else throw std::domain_error("ToHexDigit: argument not in range 0-15");
}
