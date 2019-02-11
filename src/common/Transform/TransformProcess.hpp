/*
 * TransformProcess.hpp
 *
 *  Created on: Apr 29, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMPROCESS_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMPROCESS_HPP_

#include "Transform/Transform.hpp"
#include "Process/Process.hpp"
#include "Process/Pipe.hpp"

class TransformProcess : public Transform {
public:
	Process process_;
	Pipe a_;
	Pipe b_;
	Pipe err_;

	std::string ActualCompressorVersionString() override;
	// virtual std::string ExpectedCompressorVersionString()=0;
	bool CorrectCompressorVersion() override;
	// virtual std::string HeaderMagicNumber(std::string identity)=0;
	// virtual std::string TrailerMagicNumber()=0;
	// virtual size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize)=0;
	size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize) override;
	// virtual char const *CompressionCommand()=0;
	// virtual char const *CompressionName()=0;
	// virtual std::vector<char const *> CompressionArguments()=0;
	// virtual std::vector<char const *> DecompressionArguments()=0;
	// virtual std::vector<char const *> VersionArguments()=0;
	// virtual std::string MinimumCompressionString()=0;
	std::string CompressString(std::string const & s) override;
	std::string DecompressString(std::string const & s) override;
	// virtual Transform *clone()=0;

	void OpenCompression() override;
	void OpenDecompression() override;
	// void Write(std::string const & input);
	std::string ForceWrite(std::string input) override;
	std::string Read() override;
	std::string Close() override;
	size_t WriteCount() override;
	// size_t QueuedWriteCount() override;
	size_t ReadCount() override;

	TransformProcess(std::string const & name)
		: Transform(name) {}
	virtual ~TransformProcess();
};



#endif /* SRC_COMMON_TRANSFORM_TRANSFORMPROCESS_HPP_ */
