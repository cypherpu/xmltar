/*

random_file_tree.cpp
Copyright 2020 by David A. Betz
David.Betz.MD@gmail.com
Created on: Sep 28, 2020
Author: dbetz

This file is part of xmltar.

xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xmltar.  If not, see <https://www.gnu.org/licenses/>.

*/

// File names are in UTF-8.
// Example usage:
// random_file_tree --base_path bletch --max-depth 2 --branching-factor 3 --min-files-per-dir 10 --max-files-per-dir 20 --min-filename-len 1 --max-filename-len 1 --min-file-size -1 --max-file-size 6

#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <stdexcept>
#include <vector>

#include <unicode/ustring.h>
#include <unicode/uchar.h>

#include "Utilities/Options-TarStyle.hpp"

template<typename RNG>
std::string RandomUTF8Character(RNG & rng){
	std::uniform_int_distribution<unsigned int> codePoints(0,0x10ffff);

	int32_t utf32CodePoint;
	std::string utf8CodePoint;
	UErrorCode errorCode;
	int32_t unicodeLength;

	do {
		utf32CodePoint=codePoints(rng);

		utf8CodePoint.clear();
		if (0x00<=utf32CodePoint && utf32CodePoint<=0x7f){
			char c=utf32CodePoint;
			utf8CodePoint.push_back(c);
		}
		else if (utf32CodePoint<=0x07ff){
			char c1=0xc0 | (0x1f & (utf32CodePoint>>6));
			char c2=0x80 | (0x3f & utf32CodePoint);
			utf8CodePoint.push_back(c1);
			utf8CodePoint.push_back(c2);
		}
		else if (utf32CodePoint<=0xffff){
			char c1=0xe0 | (0x0f & (utf32CodePoint>>12));
			char c2=0x80 | (0x3f & (utf32CodePoint >> 6));
			char c3=0x80 | (0x3f & utf32CodePoint);
			utf8CodePoint.push_back(c1);
			utf8CodePoint.push_back(c2);
			utf8CodePoint.push_back(c3);
		}
		else if (utf32CodePoint<=0x10ffff){
			char c1=0xf0 | (0x03 & (utf32CodePoint>>18));
			char c2=0x80 | (0x3f & (utf32CodePoint>>12));
			char c3=0x80 | (0x3f & (utf32CodePoint>>6));
			char c4=0x80 | (0x3f & utf32CodePoint);
			utf8CodePoint.push_back(c1);
			utf8CodePoint.push_back(c2);
			utf8CodePoint.push_back(c3);
			utf8CodePoint.push_back(c4);
		}
		else throw std::logic_error("RandomUTF8Character: invalid code point");

		UChar buf[9];
		int32_t destLength;
		errorCode=U_ZERO_ERROR;
		u_strFromUTF8(buf,5,&destLength,utf8CodePoint.data(),utf8CodePoint.size(),&errorCode);
		if (U_FAILURE(errorCode)){
			/*
			UCharCategory cat=(UCharCategory) u_charType(utf32CodePoint);
			std::cerr << "\t\t\t";
			switch(cat){
			case U_UNASSIGNED:
				std::cerr << "U_UNASSIGNED" << std::endl;
				break;
			case U_UPPERCASE_LETTER:
				std::cerr << "U_UPPERCASE_LETTER" << std::endl;
				break;
			case U_LOWERCASE_LETTER:
				std::cerr << "U_LOWERCASE_LETTER" << std::endl;
				break;
			case U_TITLECASE_LETTER:
				std::cerr << "U_TITLECASE_LETTER" << std::endl;
				break;
			case U_MODIFIER_LETTER:
				std::cerr << "U_MODIFIER_LETTER" << std::endl;
				break;
			case U_OTHER_LETTER:
				std::cerr << "U_OTHER_LETTER" << std::endl;
				break;
			case U_NON_SPACING_MARK:
				std::cerr << "U_NON_SPACING_MARK" << std::endl;
				break;
			case U_ENCLOSING_MARK:
				std::cerr << "U_ENCLOSING_MARK" << std::endl;
				break;
			case U_COMBINING_SPACING_MARK:
				std::cerr << "U_COMBINING_SPACING_MARK" << std::endl;
				break;
			case U_DECIMAL_DIGIT_NUMBER:
				std::cerr << "U_DECIMAL_DIGIT_NUMBER" << std::endl;
				break;
			case U_LETTER_NUMBER:
				std::cerr << "U_LETTER_NUMBER" << std::endl;
				break;
			case U_OTHER_NUMBER:
				std::cerr << "U_OTHER_NUMBER" << std::endl;
				break;
			case U_SPACE_SEPARATOR:
				std::cerr << "U_SPACE_SEPARATOR" << std::endl;
				break;
			case U_LINE_SEPARATOR:
				std::cerr << "U_LINE_SEPARATOR" << std::endl;
				break;
			case U_PARAGRAPH_SEPARATOR:
				std::cerr << "U_PARAGRAPH_SEPARATOR" << std::endl;
				break;
			case U_CONTROL_CHAR:
				std::cerr << "U_CONTROL_CHAR" << std::endl;
				break;
			case U_FORMAT_CHAR:
				std::cerr << "U_FORMAT_CHAR" << std::endl;
				break;
			case U_PRIVATE_USE_CHAR:
				std::cerr << "U_PRIVATE_USE_CHAR" << std::endl;
				break;
			case U_SURROGATE:
				std::cerr << "U_SURROGATE" << std::endl;
				break;
			case U_DASH_PUNCTUATION:
				std::cerr << "U_DASH_PUNCTUATION" << std::endl;
				break;
			case U_START_PUNCTUATION:
				std::cerr << "U_START_PUNCTUATION" << std::endl;
				break;
			case U_END_PUNCTUATION:
				std::cerr << "U_END_PUNCTUATION" << std::endl;
				break;
			case U_CONNECTOR_PUNCTUATION:
				std::cerr << "U_CONNECTOR_PUNCTUATION" << std::endl;
				break;
			case U_OTHER_PUNCTUATION:
				std::cerr << "U_OTHER_PUNCTUATION" << std::endl;
				break;
			case U_MATH_SYMBOL:
				std::cerr << "U_MATH_SYMBOL" << std::endl;
				break;
			case U_CURRENCY_SYMBOL:
				std::cerr << "U_CURRENCY_SYMBOL" << std::endl;
				break;
			case U_MODIFIER_SYMBOL:
				std::cerr << "U_MODIFIER_SYMBOL" << std::endl;
				break;
			case U_OTHER_SYMBOL:
				std::cerr << "U_OTHER_SYMBOL" << std::endl;
				break;
			case U_INITIAL_PUNCTUATION:
				std::cerr << "U_INITIAL_PUNCTUATION" << std::endl;
				break;
			case U_FINAL_PUNCTUATION:
				std::cerr << "U_FINAL_PUNCTUATION" << std::endl;
				break;
			case U_CHAR_CATEGORY_COUNT:
				std::cerr << "U_CHAR_CATEGORY_COUNT" << std::endl;
				break;
			default:
				std::cerr << "Unknown category!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
				break;
			}
			*/
			continue;
		}
		unicodeLength=u_countChar32(buf,destLength);
		// std::cerr << (int) errorCode << " " << (int) U_ZERO_ERROR << "\"" << std::boolalpha << (int) U_FAILURE(errorCode) << "\" " << destLength << " " << unicodeLength << std::endl;
	} while(U_FAILURE(errorCode) || unicodeLength!=1);

	return utf8CodePoint;
}


template<typename RNG>
std::string RandomUTF8Filename(int filenameLength, RNG & rng){
	std::string filename;

	for(int i=0; i<filenameLength; ++i){
		std::string newUTF8Character=RandomUTF8Character(rng);
		if (newUTF8Character!=std::string(1,'\0') && newUTF8Character!="/")
			filename+=RandomUTF8Character(rng);
	}

	return filename;
}

class RandomFileTreeOptions {
private:
public:
	std::filesystem::path baseDir_;
	int maxDepth_;
	int branchingFactor_;
	int minFilesPerDirectory_;
	int maxFilesPerDirectory_;
	int minFilenameLength_;
	int maxFilenameLength_;
	double log10MinFileSize_;
	double log10MaxFileSize_;

	RandomFileTreeOptions(void)
        : baseDir_("/tmp/file_tree"), maxDepth_(3), branchingFactor_(3),
		  minFilesPerDirectory_(0), maxFilesPerDirectory_(10),
		  minFilenameLength_(1), maxFilenameLength_(256),
		  log10MinFileSize_(-2), log10MaxFileSize_(6)
		{}

    ~RandomFileTreeOptions(){}

    void ProcessOptions(int argc, char const *argv[]){
    	Parse_Opts::Option_Parser p;

    	p.Add_Option(Parse_Opts::ARGS_1,"","--base_path","the directory to hold the file tree",
    			p.Assign_Args(baseDir_));
    	p.Add_Option(Parse_Opts::ARGS_1,"","--max-depth","do not exceed this depth",
    			p.Assign_Args(maxDepth_));
    	p.Add_Option(Parse_Opts::ARGS_1,"","--branching-factor","the number of subdirectories in each directory",
    			p.Assign_Args(branchingFactor_));

    	p.Add_Option(Parse_Opts::ARGS_1,"","--min-files-per-dir","the minimum number of files in each directory",
    			p.Assign_Args(minFilesPerDirectory_));
    	p.Add_Option(Parse_Opts::ARGS_1,"","--max-files-per-dir","the maximum number of files in each directory",
    			p.Assign_Args(maxFilesPerDirectory_));

    	p.Add_Option(Parse_Opts::ARGS_1,"","--min-filename-len","the minimum filename length",
    			p.Assign_Args(minFilenameLength_));
    	p.Add_Option(Parse_Opts::ARGS_1,"","--max-filename-len","the maximum filename length",
    			p.Assign_Args(maxFilenameLength_));

    	p.Add_Option(Parse_Opts::ARGS_1,"","--min-file-size","log10 of the minimum file size",
    			p.Assign_Args(log10MinFileSize_));
    	p.Add_Option(Parse_Opts::ARGS_1,"","--max-file-size","log10 of the maximum file size",
    			p.Assign_Args(log10MaxFileSize_));

    	p.Parse(argc, argv);
    }
};

class RandomFileTree {
public:
	RandomFileTreeOptions options_;

	std::mt19937_64 mt_;

	std::uniform_int_distribution<unsigned int> filesInDirectory_;
	std::uniform_int_distribution<unsigned int> filenameLength_;
	std::uniform_real_distribution<double> log10FileSize_;

	std::uniform_int_distribution<uint8_t> randomBytes_;;

	RandomFileTree(int argc, char const *argv[]){
		options_.ProcessOptions(argc,argv);

		std::random_device nonDeterministicRNG;
		std::seed_seq seed{
			nonDeterministicRNG(),
			nonDeterministicRNG(),
			nonDeterministicRNG(),
			nonDeterministicRNG(),
			nonDeterministicRNG(),
			nonDeterministicRNG(),
			nonDeterministicRNG(),
			nonDeterministicRNG()
		};
		mt_.seed(seed);

		{
			std::uniform_int_distribution<unsigned int>::param_type params(options_.minFilesPerDirectory_,options_.maxFilesPerDirectory_);
			filesInDirectory_.param(params);
		}
		{
			std::uniform_int_distribution<unsigned int>::param_type params(options_.minFilenameLength_,options_.maxFilenameLength_);
			filenameLength_.param(params);
		}
		{
			std::uniform_real_distribution<double>::param_type params(options_.log10MinFileSize_,options_.log10MaxFileSize_);
			log10FileSize_.param(params);
		}
		{
			std::uniform_int_distribution<uint8_t>::param_type params(0,255);
			randomBytes_.param(params);
		}
	}

	void PopulateDirectory(std::filesystem::path baseDir, int depth){
		++depth;
		unsigned int nDirs=options_.branchingFactor_;
		unsigned int nFiles=filesInDirectory_(mt_);

		std::vector<std::filesystem::file_type> fileTypes;
		fileTypes.reserve(nFiles);
		for(size_t i=0; i<nDirs; ++i)
			fileTypes.push_back(std::filesystem::file_type::directory);
		for(size_t i=nDirs; i<nFiles; ++i)
			fileTypes.push_back(std::filesystem::file_type::regular);
		std::shuffle(fileTypes.begin(),fileTypes.end(),mt_);

		for(auto i : fileTypes){
			if (i==std::filesystem::file_type::directory){
				std::cerr << "Directory" << std::endl;
				std::filesystem::path newDir;
				for( ; ; ){
					unsigned int nFilenameLength=filenameLength_(mt_);
					std::cerr << "nFilenameLength=" << nFilenameLength << std::endl;
					newDir=baseDir / RandomUTF8Filename(nFilenameLength,mt_);
					std::cerr << "newDir=" << newDir << std::endl;
					if (std::filesystem::exists(newDir)) continue;
					if (std::filesystem::create_directory(newDir,baseDir)) break;
				}

				if (depth<options_.maxDepth_) PopulateDirectory(newDir, depth);
			}
			else if (i==std::filesystem::file_type::regular){
				std::cerr << "File" << std::endl;
				size_t nFilesize=std::floor(std::pow(10,log10FileSize_(mt_)));
				std::filesystem::path newFile;
				for( ; ; ){
					unsigned int nFilenameLength=filenameLength_(mt_);
					std::cerr << "nFilenameLength=" << nFilenameLength << std::endl;
					newFile=baseDir / RandomUTF8Filename(nFilenameLength,mt_);
					if (std::filesystem::exists(newFile)) continue;
					std::cerr << "newFile=" << newFile << std::endl;
					std::ofstream ofs(newFile.c_str());
					if (ofs){
						for(size_t i=0; i<nFilesize; ++i)
							ofs << randomBytes_(mt_);
						break;
					}
					ofs.close();
				}
			}
		}
	}
};

int main(int argc, char const *argv[]){
	RandomFileTree randomFileTree(argc,argv);

	if (!std::filesystem::exists(randomFileTree.options_.baseDir_)){
		if (!std::filesystem::create_directories(randomFileTree.options_.baseDir_))
			throw std::runtime_error("random_file_tree: error creating directories");
	}
	else if (!std::filesystem::is_directory(std::filesystem::symlink_status(randomFileTree.options_.baseDir_)))
		throw std::runtime_error("random_file_tree: "+randomFileTree.options_.baseDir_.string()+" is not a directory");

	randomFileTree.PopulateDirectory(randomFileTree.options_.baseDir_,0);

    for(auto& p: std::filesystem::recursive_directory_iterator(randomFileTree.options_.baseDir_)){
    	for(auto c : p.path().string())
    		std::cerr << std::hex << (int)(unsigned char) c << " ";
    	std::cerr << std::endl;
    	std::cerr << p.path() << std::endl;
    }
}
