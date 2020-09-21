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

#include <json.hpp>

#include "Utilities/Crypto/DecryptXChaCha20Poly1305.hpp"
#include "Utilities/FromHex.hpp"

int main(int argc, char *argv[]){
	std::ifstream ifs("/home/dbetz/git/Private/xmltar.json");
	auto j=nlohmann::json::parse(ifs);
	std::string passphrase_=j["passphrase"];
	std::string salt_=FromEscapedHex(j["salt"]);

	std::shared_ptr<DecryptorInterface> archiveDecryption_;
	static const size_t xChaCha20Poly1305MessageLength=1<<15;
	archiveDecryption_.reset(new DecryptXChaCha20Poly1305Decorator(xChaCha20Poly1305MessageLength));

}
