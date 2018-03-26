/*
 * Compression.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: dbetz
 */

#include <stdexcept>

#include "Compression/Compression.hpp"
#include "Bidirectional_Pipe.hpp"

std::string CompressorVersion(Compression compress){
	std::string result1;
	std::string result2;
	Bidirectional_Pipe p;

	p.Open(
			CompressionCommand(compress),
			std::vector<char const *>{CompressionName(compress),"--version"});

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

bool CorrectCompressorVersion(Compression compress){
	std::string compressorVersion=CompressorVersion(compress);
	std::string expectedCompressorVersion=ExpectedCompressorVersionString(compress);

	if (compressorVersion.compare(0,expectedCompressorVersion.size(),expectedCompressorVersion)==0)
		return true;
	else
		return false;
}

std::string HeaderMagicNumber(Compression compression, std::string identity){
	switch(compression){
		case IDENTITY:
			return identity;
		case GZIP:
			return std::string("\x1f\x8b");
		case BZIP2:
			return std::string("BZh");
		case LZIP:
			return std::string("\x4c\x5a\x49\x50\x01");
		default:
			throw std::invalid_argument("XmltarOptions::HeaderMagicNumber: unrecognized Compression");
	}
}

std::string TrailerMagicNumber(Compression compression){
	switch(compression){
		case IDENTITY:
			return std::string("</members");
		case GZIP:
			return std::string("\x1f\x8b");
		case BZIP2:
			return std::string("BZh");
		case LZIP:
			return std::string("LZIP\x01");
		default:
			throw std::invalid_argument("XmltarOptions::TrailerMagicNumber: unrecognized Compression");
	}
}

size_t WorstCaseCompression(size_t size, Compression compression){
	size_t result;

	switch(compression){
	case IDENTITY:
		result=size;
		break;
	case GZIP:
		result=size+(size>>8)+50;
		break;
	case BZIP2:
		result=(size<1000?(size+(size>>1)+80):(size+(size>>7)+550));
		break;
	case LZIP:
		result=size+(size>>6)+70;
		break;
	default:
		throw std::invalid_argument("WorstCaseCompression: unknown compression");
	}

	return result;
}

char const *CompressionCommand(Compression compression){
	char const * result;

	switch(compression){
	case IDENTITY:
		result="/usr/bin/cat";
		break;
	case GZIP:
		result="/usr/bin/gzip";
		break;
	case BZIP2:
		result="/usr/bin/bzip2";
		break;
	case LZIP:
		result="/usr/bin/lzip";
		break;
	default:
		throw std::invalid_argument("CompressionCommand: unknown compression");
	}

	return result;
}

char const *CompressionName(Compression compression){
	char const * result;

	switch(compression){
	case IDENTITY:
		result="identity";
		break;
	case GZIP:
		result="gzip";
		break;
	case BZIP2:
		result="bzip2";
		break;
	case LZIP:
		result="lzip";
		break;
	default:
		throw std::invalid_argument("CompressionCommand: unknown compression");
	}

	return result;
}
std::string ExpectedCompressorVersionString(Compression compression){
	std::string result;

	switch(compression){
	case IDENTITY:
		result="cat (GNU coreutils) 8.27";
		break;
	case GZIP:
		result="gzip 1.8";
		break;
	case BZIP2:
		result="bzip2, a block-sorting file compressor.  Version 1.0.6, 6-Sept-2010.";
		break;
	case LZIP:
		result="lzip 1.20";
		break;
	default:
		throw std::invalid_argument("CompressionCommand: unknown compression");
	}

	return result;
}

std::vector<char const *> CompressionArguments(Compression compression){
	std::vector<char const *> result;

	switch(compression){
	case IDENTITY:
		result=std::vector<char const *>({"cat"});
		break;
	case GZIP:
		result=std::vector<char const *>({"gzip","-fc"});
		break;
	case BZIP2:
		result=std::vector<char const *>({"bzip2","-fc"});
		break;
	case LZIP:
		result=std::vector<char const *>({"lzip","-fc"});
		break;
	default:
		throw std::invalid_argument("CompressionCommand: unknown compression");
	}

	return result;
}

std::vector<char const *> DecompressionArguments(Compression compression){
	std::vector<char const *> result;

	switch(compression){
	case IDENTITY:
		result=std::vector<char const *>({"cat"});
		break;
	case GZIP:
		result=std::vector<char const *>({"gzip","-fcd"});
		break;
	case BZIP2:
		result=std::vector<char const *>({"bzip2","-fcd"});
		break;
	case LZIP:
		result=std::vector<char const *>({"lzip","-fcd"});
		break;
	default:
		throw std::invalid_argument("CompressionCommand: unknown compression");
	}

	return result;
}

std::string MinimumCompressionString(Compression compression){
	std::string result;

	switch(compression){
	case IDENTITY:
		result="";
		break;
	case GZIP:
		result="";
		break;
	case BZIP2:
		result="0";
		break;
	case LZIP:
		result="";
		break;
	default:
		throw std::invalid_argument("CompressionCommand: unknown compression");
	}

	return result;
}

std::string CompressString(Compression compress, std::string const & s){
	std::string result;
	Bidirectional_Pipe p;

	p.Open(
			CompressionCommand(compress),
			CompressionArguments(compress));

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

std::string DecompressString(Compression compress, std::string const & s){
	std::string result;
	Bidirectional_Pipe p;

	p.Open(
			CompressionCommand(compress),
			DecompressionArguments(compress));

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
