/*
 * TransformBzip2.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMBZIP2_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMBZIP2_HPP_

#include "Transform/Transform.hpp"

class TransformBzip2 {
	// std::string ActualCompressorVersionString();
	std::string ExpectedCompressorVersionString(){ return "bzip2, a block-sorting file compressor.  Version 1.0.6, 6-Sept-2010."; }
	// bool CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity){ return "BZh"; }
	std::string TrailerMagicNumber(){ return "BZh";	}
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize){
		return (plaintextSize<1000?(plaintextSize+(plaintextSize>>1)+80):(plaintextSize+(plaintextSize>>7)+550));
	}
	// size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	char const *CompressionCommand(){ return "/usr/bin/bzip2"; }
	char const *CompressionName(){ return "bzip2"; }
	std::vector<char const *> CompressionArguments(){ return std::vector<char const *>({"bzip2","-fc"}); }
	std::vector<char const *> DecompressionArguments(){ return std::vector<char const *>({"bzip2","-fcd"}); }
	std::vector<char const *> VersionArguments(){ return std::vector<char const *>({"bzip2","--version"}); }
	std::string MinimumCompressionString(){ return "0"; }
	// std::string CompressString(std::string const & s);		use default
	// std::string DecompressString(std::string const & s);		use default
	Transform *clone(){
		return new TransformBzip2();
	}

	~TransformGzip(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMBZIP2_HPP_ */
