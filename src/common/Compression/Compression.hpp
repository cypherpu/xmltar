/*
 * Compression.hpp
 *
 *  Created on: Nov 18, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_COMPRESSION_COMPRESSION_HPP_
#define SRC_COMMON_COMPRESSION_COMPRESSION_HPP_

#include <string>

enum Compression { IDENTITY, GZIP, BZIP2, LZIP };

std::string HeaderMagicNumber(Compression compression);
std::string TrailerMagicNumber(Compression compression);

#endif /* SRC_COMMON_COMPRESSION_COMPRESSION_HPP_ */
