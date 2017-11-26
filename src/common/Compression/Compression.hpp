/*
 * Compression.hpp
 *
 *  Created on: Nov 18, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_COMPRESSION_COMPRESSION_HPP_
#define SRC_COMMON_COMPRESSION_COMPRESSION_HPP_

#include <string>
#include <vector>

enum Compression { IDENTITY, GZIP, BZIP2, LZIP };

std::string HeaderMagicNumber(Compression compression, std::string identity);
std::string TrailerMagicNumber(Compression compression);
size_t WorstCaseCompression(size_t size, Compression compression);
char const *CompressionCommand(Compression compression);
char const *CompressionString(Compression compression);
std::vector<char const *> CompressionArguments(Compression compression);
std::vector<char const *> DecompressionArguments(Compression compression);

#endif /* SRC_COMMON_COMPRESSION_COMPRESSION_HPP_ */
