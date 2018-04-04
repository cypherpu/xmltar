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

enum class Compression { IDENTITY, GZIP, BZIP2, LZIP };

std::string CompressorVersion(Compression compression);
bool CorrectCompressorVersion(Compression compression);
std::string HeaderMagicNumber(Compression compression, std::string identity);
std::string TrailerMagicNumber(Compression compression);
size_t MaximumCompressedtextSizeGivenPlaintextSize(Compression compression, size_t plaintextSize);
size_t MinimumPlaintextSizeGivenCompressedtextSize(Compression compression, size_t compressedtextSize);
char const *CompressionCommand(Compression compression);
char const *CompressionName(Compression compression);
char const *CompressionVersionString(Compression compression);
std::string ExpectedCompressorVersionString(Compression compression);
std::vector<char const *> CompressionArguments(Compression compression);
std::vector<char const *> DecompressionArguments(Compression compression);
std::string MinimumCompressionString(Compression compression);
std::string CompressString(Compression compression, std::string const & s);
std::string DecompressString(Compression compression, std::string const & s);


#endif /* SRC_COMMON_COMPRESSION_COMPRESSION_HPP_ */
