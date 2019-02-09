/*
 * TransformProcess.hpp
 *
 *  Created on: Apr 29, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMPROCESS_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMPROCESS_HPP_

#include "Transform/Transform.hpp"
#include "Generated/BufferedBidirectionalPipe.hpp"

class TransformProcess : public Transform {
public:
	enum class PipeState { UNOPENED, OPENED };

	BufferedBidirectionalPipe pipe_;
	PipeState pipeState_;

	std::string ActualCompressorVersionString();
	// virtual std::string ExpectedCompressorVersionString()=0;
	bool CorrectCompressorVersion();
	// virtual std::string HeaderMagicNumber(std::string identity)=0;
	// virtual std::string TrailerMagicNumber()=0;
	// virtual size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize)=0;
	virtual size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	// virtual char const *CompressionCommand()=0;
	// virtual char const *CompressionName()=0;
	// virtual std::vector<char const *> CompressionArguments()=0;
	// virtual std::vector<char const *> DecompressionArguments()=0;
	// virtual std::vector<char const *> VersionArguments()=0;
	// virtual std::string MinimumCompressionString()=0;
	std::string CompressString(std::string const & s);
	std::string DecompressString(std::string const & s);
	// virtual Transform *clone()=0;

	void OpenCompression();
	void OpenDecompression();
	void Write(std::string const & input);
	std::string Read();
	std::string Close();
	size_t WriteCount();
	size_t QueuedWriteCount();
	size_t ReadCount();

	TransformProcess(std::string const & name)
		: Transform(name) {}
	virtual ~TransformProcess();
};



#endif /* SRC_COMMON_TRANSFORM_TRANSFORMPROCESS_HPP_ */
