/*
 * TransformHex.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_

#include "Transform/TransformProcess.hpp"

class TransformHex  : public TransformProcess {
public:
	// std::string ActualCompressorVersionString();
	std::string ExpectedCompressorVersionString() override { return "xxd V1.10 27oct98 by Juergen Weigert"; }
	// std::string CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity) override { return ""; }
	std::string TrailerMagicNumber() override { return ""; }
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize) override {
		// return 1+(2*plaintextSize-1)/60+2*plaintextSize;
		return (60+2*plaintextSize-1)/60+2*plaintextSize;
	}
	// size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	char const *CompressionCommand() override { return "/usr/bin/xxd"; }
	char const *CompressionName() override { return "xxd"; }
	std::vector<char const *> CompressionArguments() override { return std::vector<char const *>({"xxd","-ps"}); }
	std::vector<char const *> DecompressionArguments() override { return std::vector<char const *>({"xxd","-ps","-r"}); }
	std::vector<char const *> VersionArguments() override { return std::vector<char const *>({"xxd","-v"}); }
	size_t EmptyCompressedSize() override { throw std::logic_error("TransformHex::EmptyArchiveSize: should not be called"); }
	size_t MinimumUsableCompressedSize() override { throw std::logic_error("TransformHex::EmptyArchiveSize: should not be called"); }
	std::string MinimumCompressionString() override { return ""; }
	// std::string CompressString(std::string const & s);
	// std::string DecompressString(std::string const & s);
	Transform *clone() override { return new TransformHex(name()); }

	TransformHex(std::string const & name)
		: TransformProcess(name) {}
	~TransformHex(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_ */
