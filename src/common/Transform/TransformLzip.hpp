/*
 * TransformLzip.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMLZIP_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMLZIP_HPP_

#include "Transform/TransformProcess.hpp"

class TransformLzip : public TransformProcess {
	// std::string ActualCompressorVersionString();
	std::string ExpectedCompressorVersionString(){ return "lzip 1.20"; }
	// bool CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity){ return "LZIP\x01"; }
	std::string TrailerMagicNumber(){ return "LZIP\x01";	}
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize){
		return plaintextSize+(plaintextSize>>6)+70;
	}
	// size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	char const *CompressionCommand(){ return "/usr/bin/lzip"; }
	char const *CompressionName(){ return "lzip"; }
	std::vector<char const *> CompressionArguments(){ return std::vector<char const *>({"lzip","-fc"}); }
	std::vector<char const *> DecompressionArguments(){ return std::vector<char const *>({"lzip","-fcd"}); }
	std::vector<char const *> VersionArguments(){ return std::vector<char const *>({"lzip","--version"}); }
	size_t EmptyCompressedSize(){ return 36; }
	size_t MinimumUsableCompressedSize(){ return 36; }
	std::string MinimumCompressionString(){ return ""; }
	// std::string CompressString(std::string const & s);		use default
	// std::string DecompressString(std::string const & s);		use default
	Transform *clone(){
		return new TransformLzip();
	}

	~TransformLzip(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMLZIP_HPP_ */
