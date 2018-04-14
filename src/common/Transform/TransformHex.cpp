/*
 * TransformHex.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#include "Transform/Transform.hpp"

class TransformHex {
	virtual std::string GetCompressorVersion();
	virtual std::string HeaderMagicNumber(std::string identity);
	virtual std::string TrailerMagicNumber();
	virtual size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize);
	virtual size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	virtual char const *CompressionCommand();
	virtual char const *CompressionName();
	virtual char const *CompressionVersionString();
	virtual std::string ExpectedCompressorVersionString();
	virtual std::vector<char const *> CompressionArguments();
	virtual std::vector<char const *> DecompressionArguments();
	virtual std::string MinimumCompressionString();
	virtual std::string CompressString(std::string const & s);
	virtual std::string DecompressString(std::string const & s);

	virtual ~Transform();
};
