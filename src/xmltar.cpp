/*

xmltar.cpp
Copyright 2010-2020 David A. Betz
David.A.Betz.MD@gmail.com
Created on: 2010
Author: dbetz

This file is part of Xmltar.

Xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Xmltar.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <fstream>
#include <iostream>

#include <string>
#include <termios.h>
#include <unistd.h>

#include <boost/format.hpp>
#include <spdlog/spdlog.h>

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>

#include "Xmltar/XmltarInvocation.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Xmltar/XmltarMemberCreate.hpp"
#include "Xmltar/XmltarGlobals.hpp"

int main(int argc, char const *argv[])
{
	spdlog::set_level(spdlog::level::info);
	spdlog::set_pattern("[%Y-%m-%d] [%H:%M:%S %z] [%l] [%P] %v");

	XmltarGlobals globals;

	try {
#if 0
	    termios oldt;
	    tcgetattr(STDIN_FILENO, &oldt);
	    termios newt = oldt;
	    newt.c_lflag &= ~ECHO;
	    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	    std::string s;
	    std::cout << "Before getline" << std::endl;
	    std::getline(std::cin, s);
	    std::cout << "After getline" << std::endl;

	    std::cout << s << std::endl;

	    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);



	    //Key and IV setup
	    //AES encryption uses a secret key of a variable length (128-bit, 196-bit or 256-
	    //bit). This key is secretly exchanged between two parties before communication
	    //begins. DEFAULT_KEYLENGTH= 16 bytes
	    CryptoPP::byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
	    memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );
	    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );

	    //
	    // String and Sink setup
	    //
	    std::string plaintext = "Now is the time for all good men to come to the aide...";
	    std::string ciphertext;
	    std::string decryptedtext;

	    //
	    // Dump Plain Text
	    //
	    std::cout << "Plain Text (" << plaintext.size() << " bytes)" << std::endl;
	    std::cout << plaintext;
	    std::cout << std::endl << std::endl;

	    //
	    // Create Cipher Text
	    //
	    CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
	    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );

	    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
	    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( plaintext.c_str() ), plaintext.length() );
	    stfEncryptor.MessageEnd();

	    //
	    // Dump Cipher Text
	    //
	    std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;

	    for( int i = 0; i < ciphertext.size(); i++ ) {

	        std::cout << "0x" << std::hex << (0xFF & static_cast<CryptoPP::byte>(ciphertext[i])) << " ";
	    }

	    std::cout << std::endl << std::endl;

	    //
	    // Decrypt
	    //
	    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
	    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

	    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
	    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( ciphertext.c_str() ), ciphertext.size() );
	    stfDecryptor.MessageEnd();

	    //
	    // Dump Decrypted Text
	    //
	    std::cout << "Decrypted Text: " << std::endl;
	    std::cout << decryptedtext;
	    std::cout << std::endl << std::endl;

	    return 0;
#endif

	    globals.passphrase_="The quick brown fox jumps over t";
	    globals.key_=globals.KeyFromPassphrase(globals.passphrase_);
	    globals.key_="The quick brown fox jumps over t";

	    spdlog::debug("Before XmltarOptions");
		globals.options_.ProcessOptions(argc, argv);
	    spdlog::debug("Before XmltarInvocation");
		XmltarInvocation xmltarInvocation(globals);
	    spdlog::debug("After XmltarInvocation");
	}
	catch (char const *msg){
		std::cerr << msg << std::endl;
		exit(-1);
	}
	catch (char *msg){
		std::cerr << msg << std::endl;
		exit(-1);
	}
	catch (std::string & msg){
		std::cerr << msg << std::endl;
		exit(-1);
	}
	catch (std::exception & e){
		std::cerr << e.what() << std::endl;
		exit(-1);
	}
	catch (...){
		std::cerr << "exception thrown" << std::endl;
		exit(-1);
	}


#if 0
	std::string line, result;
	std::ifstream ifs("snapshot-template.xml");
	while(std::getline(ifs,line)){
		result+=line;
	}

	if (false)
		if (std::filesystem::exists(xmltarInvocation.Options().listed_incremental_file_.get()))
			Snapshot snapshot(xmltarInvocation.Options().listed_incremental_file_.get().string());
		else {	// create snapshot file
			if (xmltarInvocation.Options().verbosity_>=1)
				std::cerr << "xmltar: creating snapshot file " << xmltarInvocation.Options().listed_incremental_file_.get() << std::endl;
		}
	try {
	    DEBUGCXX(debugcxx,"main");

	    if (!xmltarInvocation.Options().operation_)
	    	throw std::logic_error("xmltar: called without operation");

		switch(xmltarInvocation.Options().operation_.get()){
			case XmltarOptions::CREATE:
			case XmltarOptions::APPEND:
				//xmltar.Add_To_Archive();
				break;
			case XmltarOptions::EXTRACT:
				//xmltar.Extract();
				break;
			default:
				throw ;
				break;
		}
	}
#endif

	for(auto i : globals.errorMessages_)
		std::cerr << i << std::endl;

	return globals.resultCode_;;
}
