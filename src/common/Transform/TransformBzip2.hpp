/*
 * TransformBzip2.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMBZIP2_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMBZIP2_HPP_

#include "Transform/TransformProcess.hpp"

class TransformBzip2 : public TransformProcess {
public:
	// std::string ActualCompressorVersionString();
	std::string ExpectedCompressorVersionString() override { return "bzip2, a block-sorting file compressor.  Version 1.0.6, 6-Sept-2010."; }
	// bool CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity) override { return "BZh"; }
	static std::string StaticHeaderMagicNumber(std::string identity){ return "BZh"; }
	std::string TrailerMagicNumber() override { return "BZh";	}
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize) override {
		return (plaintextSize<1000?(plaintextSize+(plaintextSize>>1)+80):(plaintextSize+(plaintextSize>>7)+550));
	}
	size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize) override {
		return compressedtextSize-(compressedtextSize>>7)-(compressedtextSize>>8)-(compressedtextSize>>9)-350;
	}
	char const *CompressionCommand() override { return "/usr/bin/bzip2"; }
	char const *CompressionName() override { return "bzip2"; }
	std::vector<char const *> CompressionArguments() override { return std::vector<char const *>({"bzip2","-fc"}); }
	std::vector<char const *> DecompressionArguments() override { return std::vector<char const *>({"bzip2","-fcd"}); }
	std::vector<char const *> VersionArguments() override { return std::vector<char const *>({"bzip2","--version"}); }
	size_t EmptyCompressedSize() override { return 14; }
	size_t MinimumUsableCompressedSize() override { return 37; }
	std::string MinimumCompressionString() override { return "0"; }
	// std::string CompressString(std::string const & s);		use default
	// std::string DecompressString(std::string const & s);		use default
	Transform *clone() override {
		return new TransformBzip2(name());
	}

	TransformBzip2(std::string const & name)
		: TransformProcess(name) {}
	~TransformBzip2(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMBZIP2_HPP_ */
