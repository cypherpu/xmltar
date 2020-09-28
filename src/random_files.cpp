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

#include "Utilities/Options-TarStyle.hpp"
#include "Utilities/Crypto/Encryptor.hpp"
#include "Utilities/Crypto/Decryptor.hpp"
#include "Utilities/Crypto/EncryptIdentity.hpp"
#include "Utilities/Crypto/DecryptIdentity.hpp"
#include "Compressors/Compressor.hpp"

class XmltarOptions {
private:
public:
	std::filesystem::path basePath_;
	int maxDepth_;
	int branchingFactor_;
	int minFilesPerDirectory_;
	int maxFilesPerDirectory_;
	int minFilenameLength_;
	int maxFilenameLength_;
	int minFileSize_;
	int maxFileSize_;

    XmltarOptions(void)
        : operation_(), verbosity_(), multi_volume_(),
		  fileCompression_(new Compressor<Identity>()),
		  fileRawCompression_(new CompressorRaw<IdentityRaw>()),
		  fileDecompression_(new Compressor<IdentityDecompress>()),
		  encoding_(new Compressor<HexEncode>()),
		  decoding_(new Compressor<HexDecode>()),
		  archiveCompression_(new Compressor<Identity>()),
		  archiveRawCompression_(new CompressorRaw<IdentityRaw>()),
		  archiveDecompression_(new Compressor<IdentityDecompress>()),
		  archiveEncryption_(new EncryptIdentityDecorator()),
		  archiveDecryption_(new DecryptIdentityDecorator()),
		  snapshotEncryption_(new EncryptIdentityDecorator()),
		  snapshotDecryption_(new DecryptIdentityDecorator()),
		  encrypted_(false),
          tape_length_(),
		  preencryptedTapeLength_(),
		  stop_after_(std::numeric_limits<size_t>::max()),
		  wait_for_space_(std::numeric_limits<size_t>::max()),
		  listed_incremental_file_(),
		  incrementalFileCompression_(new Compressor<Identity>()),
		  dump_level_(),
		  files_from_(), excludeFileGlobs_(),
		  archiveMemberTag_(),tabs_(), newlines_(),
		  base_xmltar_file_name_(), starting_volume_(), sourceFileGlobs_(), sha3_512_(false),
		  readFifo_(), writeFifo_() { }

    ~XmltarOptions(){

    }

    void ProcessOptions(int argc, char const *argv[]);
};

int main(int argc, char *argv[]){

}
