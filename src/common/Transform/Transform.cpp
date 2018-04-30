/*
 * Transform.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */
#include <stdexcept>

#include "Transform/Transform.hpp"

// std::string Transform::ActualCompressorVersionString();
// std::string Transform::ExpectedCompressorVersionString();
// bool Transform::CorrectCompressorVersion();
// std::string Transform::HeaderMagicNumber(std::string identity);
// std::string Transform::TrailerMagicNumber();
// size_t Transform::MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize);

size_t Transform::MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize){
	size_t 	plaintextSizeLB=0,
			plaintextSizeUB=compressedtextSize;

	if (compressedtextSize<MaximumCompressedtextSizeGivenPlaintextSize(0))
			throw std::invalid_argument("Transform::MinimumPlaintextSizeGivenCompressedtextSize: compressedtextSize too small");

	while(MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)<compressedtextSize)
		plaintextSizeUB+=compressedtextSize;

	while(plaintextSizeUB-plaintextSizeLB>1){
		// mid is always strictly less than plaintextSizeUB

		size_t mid=(plaintextSizeLB+plaintextSizeUB)/2;

		// plaintextSizeUB points to somewhere in the range of possible plaintext sizes

		if (MaximumCompressedtextSizeGivenPlaintextSize(mid)>=compressedtextSize)
			plaintextSizeUB=mid;

		// plaintextSizeLB points below the range of possible plaintext sizes

		else // if (MaximumCompressedtextSizeGivenPlaintextSize(mid)<compressedtextSize)
			plaintextSizeLB=mid;
	}

	return plaintextSizeUB;
}

// char const *Transform::CompressionCommand();
// char const *Transform::CompressionName();
// std::vector<char const *> Transform::CompressionArguments();
// std::vector<char const *> Transform::DecompressionArguments();
// std::string Transform::MinimumCompressionString();
// std::string Transform::CompressString(std::string const & s);
// std::string Transform::DecompressString(std::string const & s);
// void Transform::Open();
// void Transform::Write(std::string & input);
// std::string Transform::Read();
// std::string Transform::Close();

Transform::~Transform(){}
