/*

random_files.cpp
Copyright 2020 by David A. Betz
David.Betz.MD@gmail.com
Created on: Sep 28, 2020
Author: dbetz

This file is part of Assess.

Assess is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Assess is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Assess.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <stdexcept>

#include "Utilities/Options-TarStyle.hpp"
#include "Utilities/Crypto/Encryptor.hpp"
#include "Utilities/Crypto/Decryptor.hpp"
#include "Utilities/Crypto/EncryptIdentity.hpp"
#include "Utilities/Crypto/DecryptIdentity.hpp"
#include "Compressors/Compressor.hpp"

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
    }
};

class RandomFileTree {
public:
	RandomFileTreeOptions options_;

	std::mt19937_64 mt_;

	std::uniform_int_distribution<unsigned int> filesInDirectory_;
	std::uniform_int_distribution<unsigned int> filenameLength_;
	std::uniform_real_distribution<double> log10FileSize_;

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
			std::uniform_int_distribution<unsigned int>::param_type params(options_.minFilesPerDirectory_,options_.minFilesPerDirectory_);
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
	}

	void PopulateDirectory(int depth){
		unsigned int nDirs=options_.branchingFactor_;
		unsigned int nFiles=filesInDirectory_(mt_);
		unsigned int nFilenameLength=filenameLength_(mt_);
		unsigned int nFilesize=std::floor(std::pow(10,log10FileSize_(mt_)));

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

	randomFileTree.PopulateDirectory(0);
}
