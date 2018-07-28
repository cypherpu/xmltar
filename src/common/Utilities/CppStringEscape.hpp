/*
 * CppStringEscape.hpp
 *
 *  Created on: Jul 24, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_UTILITIES_CPPSTRINGESCAPE_HPP_
#define SRC_COMMON_UTILITIES_CPPSTRINGESCAPE_HPP_

#include <string>
#include <sstream>

bool IsXmlChar(char const c);
std::string ToOctal(char const c);
std::string CppStringEscape(std::string const & plaintext);

#endif /* SRC_COMMON_UTILITIES_CPPSTRINGESCAPE_HPP_ */
