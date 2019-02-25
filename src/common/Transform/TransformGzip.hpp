/*
 * TransformGzip.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMGZIP_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMGZIP_HPP_

#include "Transform/TransformProcess.hpp"

class TransformGzip : public TransformProcess {
public:
	// std::string ActualCompressorVersionString();
	std::string ExpectedCompressorVersionString() override { return "gzip 1.8"; }
	// bool CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity) override { return "\x1f\x8b"; }
	static std::string StaticHeaderMagicNumber(std::string identity){ return "\x1f\x8b"; }
	std::string TrailerMagicNumber() override { return "\x1f\x8b";	}
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize) override {
		return plaintextSize+(plaintextSize>>8)+50;
	}
	size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize) override {
		if (compressedtextSize>(compressedtextSize>>13)+(compressedtextSize>>14)+25)
			return compressedtextSize-(compressedtextSize>>13)-(compressedtextSize>>14)-25;
		else return 0;
	}
	char const *CompressionCommand() override { return "/usr/bin/gzip"; }
	char const *CompressionName() override { return "gzip"; }
	std::vector<char const *> CompressionArguments() override { return std::vector<char const *>({"gzip","-fnc"}); }
	std::vector<char const *> DecompressionArguments() override { return std::vector<char const *>({"gzip","-fcd"}); }
	std::vector<char const *> VersionArguments() override { return std::vector<char const *>({"gzip","--version"}); }
	virtual size_t EmptyCompressedSize() override { return 20; }
	virtual size_t MinimumUsableCompressedSize() override { return 20; }
	std::string MinimumCompressionString() override { return ""; }
	// std::string CompressString(std::string const & s);		use default
	// std::string DecompressString(std::string const & s);		use default
	Transform *clone() override {
		return new TransformGzip(name());
	}

	TransformGzip(std::string const & name)
		: TransformProcess(name) {}
	~TransformGzip(){
		std::cerr << "Calling TransformGzip::~TransformGzip" << std::endl;
		if (a_.readState()!=Descriptor::CLOSED)
			std::cerr << "TransformGzip::~TransformGzip: opened read state a_" << name() << std::endl;
		if (a_.writeState()!=Descriptor::CLOSED)
			std::cerr << "TransformGzip::~TransformGzip: opened write state a_" << name() << std::endl;
		if (b_.readState()!=Descriptor::CLOSED)
			std::cerr << "TransformGzip::~TransformGzip: opened read state b_" << name() << std::endl;
		if (b_.writeState()!=Descriptor::CLOSED)
			std::cerr << "TransformGzip::~TransformGzip: opened write state b_" << name() << std::endl;
	}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMGZIP_HPP_ */
