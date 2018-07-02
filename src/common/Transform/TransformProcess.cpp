/*
 * TransformProcess.cpp
 *
 *  Created on: Apr 29, 2018
 *      Author: dbetz
 */

#include "Generated/Bidirectional_Pipe.hpp"
#include "Transform/TransformProcess.hpp"

std::string TransformProcess::ActualCompressorVersionString(){
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

	// std::cerr << "\"" << result1 << "\" \"" << result2 << "\"" << std::endl;

	if (result2=="") return result1;
	else return result2;
}

// std::string TransformProcess::ExpectedCompressorVersionString();

bool TransformProcess::CorrectCompressorVersion(){
	std::string compressorVersion=ActualCompressorVersionString();
	std::string expectedCompressorVersion=ExpectedCompressorVersionString();

	if (compressorVersion.compare(0,expectedCompressorVersion.size(),expectedCompressorVersion)==0)
		return true;
	else
		return false;
}

// std::string TransformProcess::HeaderMagicNumber(std::string identity);
// std::string TransformProcess::TrailerMagicNumber();
// size_t TransformProcess::MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize);

size_t TransformProcess::MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize){
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

	std::cerr << "TransformProcess::MinimumPlaintextSizeGivenCompressedtextSize: compressedtextSize=" << compressedtextSize
			  << " plaintextSizeUB=" << plaintextSizeUB
			  << " MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)=" << MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)
			  << std::endl;

	size_t result;

	if (MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)<compressedtextSize)
		result=plaintextSizeUB;
	else
		result=plaintextSizeLB;

	return result;
}

// char const *TransformProcess::CompressionCommand();
// char const *TransformProcess::CompressionName();
// std::vector<char const *> TransformProcess::CompressionArguments();
// std::vector<char const *> TransformProcess::DecompressionArguments();
// std::string TransformProcess::MinimumCompressionString();

std::string TransformProcess::CompressString(std::string const & s){
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

std::string TransformProcess::DecompressString(std::string const & s){
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

void TransformProcess::OpenCompression(){
	pipe_.Open(
			CompressionCommand(),
			CompressionArguments());
}

void TransformProcess::OpenDecompression(){
	pipe_.Open(
			CompressionCommand(),
			DecompressionArguments());
}

void TransformProcess::Write(std::string const & input){
	if (!pipe_.ChildExitedAndAllPipesClosed() && pipe_.Can_Write())
		pipe_.QueueWrite(input);

}

std::string TransformProcess::Read(){
	std::string result;

	for(;;){
		if (pipe_.Can_Write()) pipe_.Write();
		if (pipe_.Can_Read1()) result+=pipe_.Read1();
		else if (pipe_.Can_Read2()) pipe_.Read2();
		else break;
	}

	return result;
}

std::string TransformProcess::Close(){
	std::string result;

	if (!pipe_.ChildExitedAndAllPipesClosed()){
		pipe_.QueueWriteClose();
	}
	while(!pipe_.ChildExitedAndAllPipesClosed()){
		if (pipe_.Can_Write()) pipe_.Write();
		if (pipe_.Can_Read1()) result+=pipe_.Read1();
		if (pipe_.Can_Read2()) pipe_.Read2();
	}

	return result;
}

size_t TransformProcess::WriteCount(){
	return pipe_.Write_Count();
}

size_t TransformProcess::ReadCount(){
	return pipe_.Read1_Count();
}

TransformProcess::~TransformProcess(){}
