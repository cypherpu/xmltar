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

	std::string ActualCompressorVersionString() override { return "identity"; }
	std::string ExpectedCompressorVersionString() override { return "identity"; }
	bool CorrectCompressorVersion() override { return true; }
	std::string HeaderMagicNumber(std::string identity) override { return ""; }
	static std::string StaticHeaderMagicNumber(std::string identity){ return identity; }
	std::string TrailerMagicNumber() override { return ""; }
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize) override { return plaintextSize; }
	size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize) override { return compressedtextSize; }
	char const *CompressionCommand() override { return "identity"; }
	char const *CompressionName() override { return "identity"; }
	std::vector<char const *> CompressionArguments() override { return std::vector<char const *>(); }
	std::vector<char const *> DecompressionArguments() override { return std::vector<char const *>(); }
	std::vector<char const *> VersionArguments() override { return std::vector<char const *>(); }
	size_t EmptyCompressedSize() override { return 0; }
	size_t MinimumUsableCompressedSize() override { return 0; }
	std::string MinimumCompressionString() override { return ""; }
	std::string CompressString(std::string const & s) override { return s; }
	std::string DecompressString(std::string const & s) override { return s; }
	Transform *clone(){
		return new TransformIdentity(name());
	}

	void OpenCompression() override { if (data_!="") throw std::logic_error("TransformIdentity::OpenCompression: data_ not empty"); }
	void OpenDecompression() override { if (data_!="") throw std::logic_error("TransformIdentity::OpenDecompression: data_ not empty"); }
	// void Write(std::string const & input){ data_+=input; write_count+=input.size(); }
	std::string ForceWrite(std::string data) override { write_count+=data.size(); read_count+=data.size(); return data; }
	std::string Read() override { std::string tmp; std::swap(tmp,data_); read_count+=tmp.size(); return tmp; }
	std::string ForceWriteAndClose(std::string input) override { return ForceWrite(input); }
	size_t ReadCount() override { return read_count; }
	size_t WriteCount() override { return write_count; }

	TransformIdentity(std::string const & name)
		: Transform(name), read_count(0), write_count(0) {}
	~TransformIdentity(){}
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMIDENTITY_HPP_ */
