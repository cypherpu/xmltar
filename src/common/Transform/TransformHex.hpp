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
	std::string ExpectedCompressorVersionString(){ return "xxd V1.10 27oct98 by Juergen Weigert"; }
	// std::string CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity){ return ""; }
	std::string TrailerMagicNumber(){ return ""; }
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize){
		// return 1+(2*plaintextSize-1)/60+2*plaintextSize;
		return (60+2*plaintextSize-1)/60+2*plaintextSize;
	}
	// size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	char const *CompressionCommand(){ return "/usr/bin/xxd"; }
	char const *CompressionName(){ return "xxd"; }
	std::vector<char const *> CompressionArguments(){ return std::vector<char const *>({"xxd","-ps"}); }
	std::vector<char const *> DecompressionArguments(){ return std::vector<char const *>({"xxd","-ps","-r"}); }
	std::vector<char const *> VersionArguments(){ return std::vector<char const *>({"xxd","-v"}); }
	size_t EmptyCompressedSize(){ throw std::logic_error("TransformHex::EmptyArchiveSize: should not be called"); }
	size_t MinimumUsableCompressedSize(){ throw std::logic_error("TransformHex::EmptyArchiveSize: should not be called"); }
	std::string MinimumCompressionString(){ return ""; }
	// std::string CompressString(std::string const & s);
	// std::string DecompressString(std::string const & s);
	Transform *clone(){ return new TransformHex(name()); }

	TransformHex(std::string const & name)
		: TransformProcess(name) {}
	~TransformHex(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_ */
