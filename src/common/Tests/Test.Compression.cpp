/*
 * Test.Compression.cpp
 *
 *  Created on: Nov 23, 2017
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Generated/Bidirectional_Pipe.hpp"
#include "Transform/TransformHex.hpp"

TEST(XmltarTest,CompressHex)
{
	TransformHex transformHex("CompressHex");
	for(int i=0; i<500; ++i){
		std::string cleartext(i,'0');
		//std::cerr << "\"" << cleartext << "\"" << std::endl;
		std::string compressedtext=transformHex.CompressString(cleartext);
		//std::cerr << "\"" << compressedtext << "\"" << std::endl;
		//std::cerr << transformHex.MaximumCompressedtextSizeGivenPlaintextSize(cleartext.size()) << " " << compressedtext.size() << std::endl;

		ASSERT_TRUE(transformHex.MaximumCompressedtextSizeGivenPlaintextSize(cleartext.size())==compressedtext.size());
	}
}
