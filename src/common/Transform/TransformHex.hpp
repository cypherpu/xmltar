/*
 * TransformHex.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_

#include "Transform/Transform.hpp"

class TransformHex  : public Transform {
	// std::string ActualCompressorVersionString();							use default
	std::string ExpectedCompressorVersionString(){ return "xxd V1.10 27oct98 by Juergen Weigert"; }
	std::string GetCompressorVersion();
	std::string HeaderMagicNumber(std::string identity);
	std::string TrailerMagicNumber();
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize);
	size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	char const *CompressionCommand();
	char const *CompressionName();
	std::vector<char const *> CompressionArguments();
	std::vector<char const *> DecompressionArguments();
	std::string MinimumCompressionString();
	std::string CompressString(std::string const & s);
	std::string DecompressString(std::string const & s);
	Transform *clone();

	~TransformHex();
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_ */
