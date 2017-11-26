/*
 * Compression.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: dbetz
 */

#include <stdexcept>

#include "Compression/Compression.hpp"

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
