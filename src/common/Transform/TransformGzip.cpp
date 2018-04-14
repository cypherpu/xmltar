/*
 * TransformGzip.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#include "Transform/TransformGzip.hpp"

std::string Transform::ExpectedCompressorVersionString(){
	return "gzip 1.8";
}

std::string TransformGzip::HeaderMagicNumber(std::string identity){
	return "\x1f\x8b";
}

std::string Transform::TrailerMagicNumber(){
	return "\x1f\x8b";
}
size_t Transform::MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize){
	return plaintextSize+(plaintextSize>>8)+50;
}

size_t Transform::MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize){

}

char const *Transform::CompressionCommand(){
	return "/usr/bin/gzip";
}

char const *Transform::CompressionName(){
	return "gzip";
}

std::vector<char const *> Transform::CompressionArguments(){
	return std::vector<char const *>({"gzip","-fc"});
}

std::vector<char const *> Transform::DecompressionArguments(){
	result=std::vector<char const *>({"gzip","-fcd"});
}

std::string Transform::MinimumCompressionString()
{
	return "";
}

Transform *Transform::clone(){
	return *this;
}

Transform::~Transform();
