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
	size_t read_count;
	size_t write_count;

	std::string ActualCompressorVersionString(){ return "identity"; }
	std::string ExpectedCompressorVersionString(){ return "identity"; }
	bool CorrectCompressorVersion(){ return true; }
	std::string HeaderMagicNumber(std::string identity){ return ""; }
	static std::string StaticHeaderMagicNumber(std::string identity){ return identity; }
	std::string TrailerMagicNumber(){ return ""; }
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize){ return plaintextSize; }
	size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize){ return compressedtextSize; }
	char const *CompressionCommand(){ return "identity"; }
	char const *CompressionName(){ return "identity"; }
	std::vector<char const *> CompressionArguments(){ return std::vector<char const *>(); }
	std::vector<char const *> DecompressionArguments(){ return std::vector<char const *>(); }
	std::vector<char const *> VersionArguments(){ return std::vector<char const *>(); }
	size_t EmptyCompressedSize(){ return 0; }
	size_t MinimumUsableCompressedSize(){ return 0; }
	std::string MinimumCompressionString(){ return ""; }
	std::string CompressString(std::string const & s){ return s; }
	std::string DecompressString(std::string const & s){ return s; }
	Transform *clone(){
		return new TransformIdentity(name());
	}

	void OpenCompression(){ if (data_!="") throw std::logic_error("TransformIdentity::OpenCompression: data_ not empty"); }
	void OpenDecompression(){ if (data_!="") throw std::logic_error("TransformIdentity::OpenDecompression: data_ not empty"); }
	void Write(std::string const & input){ data_+=input; write_count+=input.size(); }
	std::string Read(){ std::string tmp; std::swap(tmp,data_); read_count+=tmp.size(); return tmp; }
	std::string Close(){ std::string tmp; std::swap(tmp,data_); read_count+=tmp.size(); return tmp; }
	size_t ReadCount(){ return read_count; }
	size_t WriteCount(){ return write_count; }
	size_t QueuedWriteCount(){ return write_count; }

	TransformIdentity(std::string const & name)
		: Transform(name), read_count(0), write_count(0) {}
	~TransformIdentity(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMIDENTITY_HPP_ */
