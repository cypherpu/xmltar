/*

decrypt.cpp
Copyright 2020 by David A. Betz
David.Betz.MD@gmail.com
Created on: Sep 21, 2020
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

#include <fstream>

#include <nlohmann/json.hpp>

#include "Xmltar/XmltarGlobals.hpp"

#include "Utilities/Crypto/DecryptXChaCha20Poly1305.hpp"
#include "Utilities/Crypto/GenerateKey.hpp"
#include "Utilities/FromHex.hpp"

int main(int argc, char *argv[]){
	if (argc!=2)
		std::runtime_error("Usage: decrypt filename");

	std::string passphrase;
	std::string salt;
	{
		std::ifstream ifs("/home/dbetz/git/Private/xmltar.json");
		auto j=nlohmann::json::parse(ifs);
		passphrase=j["passphrase"];
		salt=FromEscapedHex(j["salt"]);
	}

	std::ifstream ifs(argv[1]);
	std::shared_ptr<DecryptorInterface> archiveDecryption;
	static const size_t xChaCha20Poly1305MessageLength=1<<15;
	archiveDecryption.reset(new DecryptXChaCha20Poly1305Decorator(xChaCha20Poly1305MessageLength));
	std::cout << archiveDecryption->Open(GenerateKey(passphrase,salt));
	char buffer[1024];
	while(ifs){
		ifs.read(buffer,sizeof(buffer)/sizeof(*buffer));

		std::cout << archiveDecryption->Decrypt(std::string(buffer,ifs.gcount()));
	}

	std::cout << archiveDecryption->Close();

	ifs.close();
}
