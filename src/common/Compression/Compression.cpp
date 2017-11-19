/*
 * Compression.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: dbetz
 */

#include "Compression/Compression.hpp"

std::string HeaderMagicNumber(Compression compression){
	switch(compression){
		case IDENTITY:
			return std::string("<?xml");
		case GZIP:
			return std::string("\x1f\x8b");
		case BZIP2:
			return std::string("BZh");
		case LZIP:
			return std::string("\x4c\x5a\x49\x50\x01");
		default:
			throw "XmltarOptions::HeaderMagicNumber: unrecognized Compression";
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
			throw "XmltarOptions::TrailerMagicNumber: unrecognized Compression";
	}
}
