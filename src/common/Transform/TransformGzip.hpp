/*
 * TransformGzip.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMGZIP_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMGZIP_HPP_

#include "Transform/Transform.hpp"

class TransformGzip {
	// std::string ActualCompressorVersionString();
	std::string ExpectedCompressorVersionString(){ return "gzip 1.8"; }
	// bool CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity){ return "\x1f\x8b"; }
	std::string TrailerMagicNumber(){ return "\x1f\x8b";	}
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize){
		return plaintextSize+(plaintextSize>>8)+50;
	}
	// size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	char const *CompressionCommand(){ return "/usr/bin/gzip"; }
	char const *CompressionName(){ return "gzip"; }
	std::vector<char const *> CompressionArguments(){ return std::vector<char const *>({"gzip","-fc"}); }
	std::vector<char const *> DecompressionArguments(){ return std::vector<char const *>({"gzip","-fcd"}); }
	std::vector<char const *> VersionArguments(){ return std::vector<char const *>({"gzip","--version"}); }
	std::string MinimumCompressionString(){ return ""; }
	// std::string CompressString(std::string const & s);		use default
	// std::string DecompressString(std::string const & s);		use default
	Transform *clone(){
		return new TransformGzip();
	}

	~TransformGzip(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMGZIP_HPP_ */
