/*
 * TransformProcess.cpp
 *
 *  Created on: Apr 29, 2018
 *      Author: dbetz
 */

#include <iostream>
#include <sstream>

#include "Generated/Process/Connection.hpp"
#include "Generated/Process/Process.hpp"
#include "Generated/Process/Utilities.hpp"

#include "Transform/TransformProcess.hpp"

#include "../Debug2/Debug2.hpp"

std::string TransformProcess::ActualCompressorVersionString(){
	std::istringstream iss;
	std::ostringstream oss;

	Process compress(CompressionCommand(),std::vector<char const *>{CompressionName(),"--version"},"");
	Chain1e(compress,iss,oss);

	return oss.str();
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
	std::cerr << dbg << ": compressedtextSize=" << compressedtextSize << std::endl;
	size_t 	plaintextSizeLB=0,
			plaintextSizeUB=compressedtextSize;

	if (compressedtextSize<MaximumCompressedtextSizeGivenPlaintextSize(0)){
		return 0;

		std::cerr << dbg << ": compressedtextSize=" << compressedtextSize << std::endl;
		std::cerr << dbg << ": MaximumCompressedtextSizeGivenPlaintextSize(0)=" << MaximumCompressedtextSizeGivenPlaintextSize(0) << std::endl;
		throw std::invalid_argument("Transform::MinimumPlaintextSizeGivenCompressedtextSize: compressedtextSize too small");
	}

	std::cerr << dbg << ": compressedtextSize=" << compressedtextSize << " < MaximumCompressedtextSizeGivenPlaintextSize(0)=" << MaximumCompressedtextSizeGivenPlaintextSize(0) << std::endl;
	std::cerr << dbg << ": plaintextSizeLB=" << plaintextSizeLB << " plaintextSizeUB=" << plaintextSizeUB << std::endl;

	std::cerr << dbg << ": 1" << std::endl;

	while(MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)<compressedtextSize){
		std::cerr << dbg << ":1: plaintextSizeLB=" << plaintextSizeLB << " plaintextSizeUB=" << plaintextSizeUB << std::endl;
		plaintextSizeUB+=compressedtextSize;
	}

	//std::cerr << dbg << ": 2" << std::endl;

	while(plaintextSizeUB-plaintextSizeLB>1){
		std::cerr << dbg << ":2: plaintextSizeLB=" << plaintextSizeLB << " plaintextSizeUB=" << plaintextSizeUB << std::endl;

		// mid is always strictly less than plaintextSizeUB

		size_t mid=(plaintextSizeLB+plaintextSizeUB)/2;

		// plaintextSizeUB points to somewhere in the range of possible plaintext sizes

		if (MaximumCompressedtextSizeGivenPlaintextSize(mid)>=compressedtextSize)
			plaintextSizeUB=mid;

		// plaintextSizeLB points below the range of possible plaintext sizes

		else // if (MaximumCompressedtextSizeGivenPlaintextSize(mid)<compressedtextSize)
			plaintextSizeLB=mid;
	}

	std::cerr << dbg << ": compressedtextSize=" << compressedtextSize << std::endl;
	std::cerr << dbg << ": plaintextSizeUB=" << plaintextSizeUB << std::endl;
	std::cerr << dbg << ": MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)=" << MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB) << std::endl;

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
	std::istringstream iss(s);
	std::ostringstream oss;

	Process compress(CompressionCommand(),CompressionArguments(),"xxd");
	Chain1(compress,iss,oss);

	return oss.str();
}

std::string TransformProcess::DecompressString(std::string const & s){
	std::istringstream iss(s);
	std::ostringstream oss;

	Process compress(CompressionCommand(),DecompressionArguments(),"xxd");
	Chain1(compress,iss,oss);

	return oss.str();
}

void TransformProcess::OpenCompression(){
	process_.Initialize(
			CompressionCommand(),
			CompressionArguments());

	launch({
			{a_,EndPointAction::CLOSE,EndPointAction::NONBLOCKING_WRITE},
			{process_,a_,EndPointAction::STDIN,EndPointAction::CLOSE},
			{b_,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{process_,b_,EndPointAction::CLOSE,EndPointAction::STDOUT},

			{err_,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{process_,err_,EndPointAction::CLOSE,EndPointAction::STDERR},
		   }
	      );
}

void TransformProcess::OpenDecompression(){
	process_.Initialize(
			CompressionCommand(),
			DecompressionArguments());

	launch({
			{a_,EndPointAction::CLOSE,EndPointAction::NONBLOCKING_WRITE},
			{process_,a_,EndPointAction::STDIN,EndPointAction::CLOSE},
			{b_,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{process_,b_,EndPointAction::CLOSE,EndPointAction::STDOUT},

			{err_,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{process_,err_,EndPointAction::CLOSE,EndPointAction::STDERR},
		   }
	      );
}

std::string TransformProcess::ForceWrite(std::string input){
	std::string output;

	input=a_.write(input);
	while(input.size()){
		output+=b_.read();
		input=a_.write(input);
	}

	return output;
}

std::string TransformProcess::ForceWriteAndClose(std::string input){
	std::string result;

	spdlog::debug("Before ForceWrite");
	result=ForceWrite(input);
	a_.closeWrite();

	spdlog::debug("Before while");
	while(process_){
		spdlog::debug("while process read={} write={}", b_.readState(), a_.writeState());
		result+=b_.read();
		err_.read();
	}

	spdlog::debug("Before err_.read");
	std::string tmp;
	while((tmp=b_.read()).size())
		result+=tmp;
	err_.read();

	b_.closeRead();
	err_.closeRead();

	return result;
}

size_t TransformProcess::WriteCount(){
	return a_.writeCount();
}

size_t TransformProcess::ReadCount(){
	return b_.readCount();
}

TransformProcess::~TransformProcess(){
}

