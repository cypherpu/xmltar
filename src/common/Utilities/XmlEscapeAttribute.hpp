/*
 * XMLEscapeAttribute.hpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_UTILITIES_XMLESCAPEATTRIBUTE_HPP_
#define SRC_COMMON_UTILITIES_XMLESCAPEATTRIBUTE_HPP_

#include <string>

std::string XmlEscapeAttribute(std::string const data);
std::string XmlUnescapeAttribute(std::string const data);

#endif /* SRC_COMMON_UTILITIES_XMLESCAPEATTRIBUTE_HPP_ */
