/*
 * TransformProcess.cpp
 *
 *  Created on: Apr 29, 2018
 *      Author: dbetz
 */

#include <iostream>

#include "Generated/BufferedBidirectionalPipe.hpp"
#include "Transform/TransformProcess.hpp"

#include "../Debug2/Debug2.hpp"

std::string TransformProcess::ActualCompressorVersionString(){
	std::string result1;
	std::string result2;
	BufferedBidirectionalPipe p;

	p.Open(
			CompressionCommand(),
			std::vector<char const *>{CompressionName(),"--version"});

	if (p){
		p.Buffered_Write("");
		p.Buffered_close_write();
	}
	while(p){
		if (p.Buffered_Can_Read1()) result1+=p.Buffered_Read1();
		if (p.Buffered_Can_Read2()) result2+=p.Buffered_Read2();
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

size_t TransformProcess::MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize){	//FIXME - duplicated in Transform.cpp
	betz::Debug2 dbg("TransformProcess::MinimumPlaintextSizeGivenCompressedtextSize");

	size_t 	plaintextSizeLB=0,
			plaintextSizeUB=compressedtextSize;

	if (compressedtextSize<MaximumCompressedtextSizeGivenPlaintextSize(0)){
		return 0;

		std::cerr << dbg << "compressedtextSize=" << compressedtextSize << " < MaximumCompressedtextSizeGivenPlaintextSize(0)=" << MaximumCompressedtextSizeGivenPlaintextSize(0) << std::endl;
		throw std::invalid_argument("Transform::MinimumPlaintextSizeGivenCompressedtextSize: compressedtextSize too small");
	}

	//std::cerr << dbg << "compressedtextSize=" << compressedtextSize << " < MaximumCompressedtextSizeGivenPlaintextSize(0)=" << MaximumCompressedtextSizeGivenPlaintextSize(0) << std::endl;
	//std::cerr << dbg << ": plaintextSizeLB=" << plaintextSizeLB << " plaintextSizeUB=" << plaintextSizeUB << std::endl;

	//std::cerr << dbg << ": 1" << std::endl;

	while(MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)<compressedtextSize)
		plaintextSizeUB+=compressedtextSize;

	//std::cerr << dbg << ": 2" << std::endl;

	while(plaintextSizeUB-plaintextSizeLB>1){
		//std::cerr << dbg << ": plaintextSizeLB=" << plaintextSizeLB << " plaintextSizeUB=" << plaintextSizeUB << std::endl;

		// mid is always strictly less than plaintextSizeUB

		size_t mid=(plaintextSizeLB+plaintextSizeUB)/2;

		// plaintextSizeUB points to somewhere in the range of possible plaintext sizes

		if (MaximumCompressedtextSizeGivenPlaintextSize(mid)>=compressedtextSize)
			plaintextSizeUB=mid;

		// plaintextSizeLB points below the range of possible plaintext sizes

		else // if (MaximumCompressedtextSizeGivenPlaintextSize(mid)<compressedtextSize)
			plaintextSizeLB=mid;
	}

	std::cerr << dbg << ": compressedtextSize=" << compressedtextSize
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
	BufferedBidirectionalPipe p;

	p.Open(
			CompressionCommand(),
			CompressionArguments());

	if (p){
		p.Buffered_Write(s);
		p.Buffered_close_write();
	}
	while(p){
		if (p.Buffered_Can_Read1()) result+=p.Buffered_Read1();
		if (p.Buffered_Can_Read2()) p.Buffered_Read2();
	}

	return result;
}

std::string TransformProcess::DecompressString(std::string const & s){
	std::string result;
	BufferedBidirectionalPipe p;

	p.Open(
			CompressionCommand(),
			DecompressionArguments());

	if (p){
		p.Buffered_Write(s);
		p.Buffered_close_write();
	}
	while(p){
		if (p.Buffered_Can_Read1()) result+=p.Buffered_Read1();
		if (p.Buffered_Can_Read2()) p.Buffered_Read2();
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
	// if (!pipe_.ChildExitedAndAllPipesClosed() && pipe_.Can_Write())
		pipe_.Buffered_Write(input);

}

std::string TransformProcess::Read(){
	std::string result;

	if (pipe_)
		if (pipe_.Buffered_Can_Read1()) result+=pipe_.Buffered_Read1();
		else if (pipe_.Buffered_Can_Read2()) pipe_.Buffered_Read2();

#if 0
	for(;;){
		pipe_.Select_Nonblocking();
		if (pipe_.Can_Write()) pipe_.Write();
		if (pipe_.Can_Read1()) result+=pipe_.Read1();
		else if (pipe_.Can_Read2()) pipe_.Read2();
		else break;
	}
#endif
	return result;
}

std::string TransformProcess::Close(){
	std::string result;

	if (pipe_) pipe_.Buffered_close_write();

	while(pipe_){
		if (pipe_.Buffered_Can_Read1()) result+=pipe_.Buffered_Read1();
		if (pipe_.Buffered_Can_Read2()) pipe_.Buffered_Read2();
	}

	return result;
}

size_t TransformProcess::WriteCount(){
	return pipe_.WriteCount();
}

size_t TransformProcess::QueuedWriteCount(){
	return pipe_.BufferedWriteCount();
}

size_t TransformProcess::ReadCount(){
	// return pipe_.Read1_Count();
	return pipe_.Read1Count();
}

TransformProcess::~TransformProcess(){}
