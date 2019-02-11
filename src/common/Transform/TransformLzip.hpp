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
public:
	// std::string ActualCompressorVersionString();
	std::string ExpectedCompressorVersionString() override { return "lzip 1.20"; }
	// bool CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity) override { return "LZIP\x01"; }
	static std::string StaticHeaderMagicNumber(std::string identity){ return "LZIP\x01"; }
	std::string TrailerMagicNumber() override { return "LZIP\x01";	}
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize) override {
		return plaintextSize+(plaintextSize>>6)+70;
	}
	size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize) override {
		return compressedtextSize-(compressedtextSize>>7)-(compressedtextSize>>8)-(compressedtextSize>>9)-50;
	}
	char const *CompressionCommand() override { return "/usr/bin/lzip"; }
	char const *CompressionName() override { return "lzip"; }
	std::vector<char const *> CompressionArguments() override { return std::vector<char const *>({"lzip","-fc"}); }
	std::vector<char const *> DecompressionArguments() override { return std::vector<char const *>({"lzip","-fcd"}); }
	std::vector<char const *> VersionArguments() override { return std::vector<char const *>({"lzip","--version"}); }
	size_t EmptyCompressedSize() override { return 36; }
	size_t MinimumUsableCompressedSize() override { return 36; }
	std::string MinimumCompressionString() override { return ""; }
	// std::string CompressString(std::string const & s);		use default
	// std::string DecompressString(std::string const & s);		use default
	Transform *clone(){
		return new TransformLzip(name());
	}

	TransformLzip(std::string const & name)
		: TransformProcess(name){}
	~TransformLzip(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMLZIP_HPP_ */
