/*
 * Transform.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORM_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORM_HPP_

#include <string>
#include <vector>

class Transform {
	std::string name_;
public:
	virtual std::string ActualCompressorVersionString()=0;
	virtual std::string ExpectedCompressorVersionString()=0;
	virtual bool CorrectCompressorVersion()=0;
	virtual std::string HeaderMagicNumber(std::string identity)=0;
	virtual std::string TrailerMagicNumber()=0;
	virtual size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize)=0;
	virtual size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize)=0;
	virtual char const *CompressionCommand()=0;
	virtual char const *CompressionName()=0;
	virtual std::vector<char const *> CompressionArguments()=0;
	virtual std::vector<char const *> DecompressionArguments()=0;
	virtual std::vector<char const *> VersionArguments()=0;
	virtual size_t EmptyCompressedSize()=0;
	virtual size_t MinimumUsableCompressedSize()=0;
	virtual std::string MinimumCompressionString()=0;
	virtual std::string CompressString(std::string const & s)=0;
	virtual std::string DecompressString(std::string const & s)=0;
	virtual Transform *clone()=0;

	virtual void OpenCompression()=0;
	virtual void OpenDecompression()=0;
	// virtual void Write(std::string const & input)=0;
	virtual std::string ForceWrite(std::string input)=0;
	virtual std::string Read()=0;
	virtual std::string ForceWriteAndClose(std::string input)=0;
	virtual size_t WriteCount()=0;
	// virtual size_t QueuedWriteCount()=0;
	virtual size_t ReadCount()=0;

	std::string name(){ return name_; }

	Transform(std::string const & name)
		: name_(name){}
	virtual ~Transform();
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORM_HPP_ */
