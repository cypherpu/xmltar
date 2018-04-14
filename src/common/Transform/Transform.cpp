/*
 * Transform.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#include "Generated/Bidirectional_Pipe.hpp"
#include "Transform/Transform.hpp"

std::string Transform::ActualCompressorVersionString(){
	std::string result1;
	std::string result2;
	Bidirectional_Pipe p;

	p.Open(
			CompressionCommand(),
			std::vector<char const *>{CompressionName(),"--version"});

	if (!p.ChildExitedAndAllPipesClosed() && p.Can_Write()){
		p.QueueWrite("");
		p.QueueWriteClose();
	}
	while(!p.ChildExitedAndAllPipesClosed()){
		if (p.Can_Write()) p.Write();
		if (p.Can_Read1()) result1+=p.Read1();
		if (p.Can_Read2()) result2+=p.Read2();
	}

	std::cerr << "\"" << result1 << "\" \"" << result2 << "\"" << std::endl;

	if (result2=="") return result1;
	else return result2;
}

// std::string Transform::ExpectedCompressorVersionString();

bool Transform::CorrectCompressorVersion(){
	std::string compressorVersion=ActualCompressorVersionString();
	std::string expectedCompressorVersion=ExpectedCompressorVersionString();

	if (compressorVersion.compare(0,expectedCompressorVersion.size(),expectedCompressorVersion)==0)
		return true;
	else
		return false;
}

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

std::string Transform::CompressString(std::string const & s){
	std::string result;
	Bidirectional_Pipe p;

	p.Open(
			CompressionCommand(),
			CompressionArguments());

	if (!p.ChildExitedAndAllPipesClosed() && p.Can_Write()){
		p.QueueWrite(s);
		p.QueueWriteClose();
	}
	while(!p.ChildExitedAndAllPipesClosed()){
		if (p.Can_Write()) p.Write();
		if (p.Can_Read1()) result+=p.Read1();
		if (p.Can_Read2()) p.Read2();
	}

	return result;
}

std::string Transform::DecompressString(std::string const & s){
	std::string result;
	Bidirectional_Pipe p;

	p.Open(
			CompressionCommand(),
			DecompressionArguments());

	if (!p.ChildExitedAndAllPipesClosed() && p.Can_Write()){
		p.QueueWrite(s);
		p.QueueWriteClose();
	}
	while(!p.ChildExitedAndAllPipesClosed()){
		if (p.Can_Read1()) result+=p.Read1();
		if (p.Can_Read2()) p.Read2();
	}

	return result;
}

Transform::~Transform(){}
