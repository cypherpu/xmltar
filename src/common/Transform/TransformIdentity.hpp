/*
 * TransformIdentity.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMIDENTITY_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMIDENTITY_HPP_

#include <utility>

#include "Transform/Transform.hpp"

class TransformIdentity : public Transform {
public:
	std::string data_;

	std::string ActualCompressorVersionString(){ return "Identity"; }
	std::string ExpectedCompressorVersionString(){ return "Identity"; }
	bool CorrectCompressorVersion(){ return true; }
	std::string HeaderMagicNumber(std::string identity){ return ""; }
	std::string TrailerMagicNumber(){ return ""; }
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize){ return plaintextSize; }
	size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize){ return compressedtextSize; }
	char const *CompressionCommand(){ return "Identity"; }
	char const *CompressionName(){ return "Identity"; }
	std::vector<char const *> CompressionArguments(){ return std::vector<char const *>(); }
	std::vector<char const *> DecompressionArguments(){ return std::vector<char const *>(); }
	std::vector<char const *> VersionArguments(){ return std::vector<char const *>(); }
	std::string MinimumCompressionString(){ return ""; }
	std::string CompressString(std::string const & s){ return s; }
	std::string DecompressString(std::string const & s){ return s; }
	Transform *clone(){
		return new TransformIdentity();
	}

	void OpenCompression(){ if (data_!="") throw std::logic_error("TransformIdentity::OpenCompression: data_ not empty"); }
	void OpenDecompression(){ if (data_!="") throw std::logic_error("TransformIdentity::OpenDecompression: data_ not empty"); }
	void Write(std::string & input){ data_+=input; }
	std::string Read(){ std::string tmp; std::swap(tmp,data_); return tmp; }
	std::string Close(){ std::string tmp; std::swap(tmp,data_); return tmp; }

	~TransformIdentity(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMIDENTITY_HPP_ */
