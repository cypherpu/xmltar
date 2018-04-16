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
public:
	virtual std::string ActualCompressorVersionString();
	virtual std::string ExpectedCompressorVersionString()=0;
	virtual bool CorrectCompressorVersion();
	virtual std::string HeaderMagicNumber(std::string identity)=0;
	virtual std::string TrailerMagicNumber()=0;
	virtual size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize)=0;
	virtual size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	virtual char const *CompressionCommand()=0;
	virtual char const *CompressionName()=0;
	virtual std::vector<char const *> CompressionArguments()=0;
	virtual std::vector<char const *> DecompressionArguments()=0;
	virtual std::vector<char const *> VersionArguments()=0;
	virtual std::string MinimumCompressionString()=0;
	virtual std::string CompressString(std::string const & s);
	virtual std::string DecompressString(std::string const & s);
	virtual Transform *clone()=0;

	virtual ~Transform();
};

#endif /* SRC_COMMON_TRANSFORM_TRANSFORM_HPP_ */
