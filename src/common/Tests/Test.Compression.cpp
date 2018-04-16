/*
 * Test.Compression.cpp
 *
 *  Created on: Nov 23, 2017
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Compression/Compression.hpp"
#include "Generated/Bidirectional_Pipe.hpp"
#include "Transform/TransformHex.hpp"

TEST(XmltarTest,CompressGzip)
{
	std::string cleartext("<padding>0123456789abcdef</padding>");
	std::string compressedCleartext;
	std::string retrievedCleartext;
	{
		Bidirectional_Pipe p(CompressionCommand(Compression::GZIP),CompressionArguments(Compression::GZIP));

		if (!p.ChildExitedAndAllPipesClosed() && p.Can_Write()){
			p.QueueWrite(cleartext);
			p.QueueWriteClose();
		}
		while(!p.ChildExitedAndAllPipesClosed()){
			if (p.Can_Read1()) compressedCleartext+=p.Read1();
			if (p.Can_Read2()) p.Read2();
		}
		ASSERT_EQ(p.ExitStatus(),0);
	}
	{
		Bidirectional_Pipe p(CompressionCommand(Compression::GZIP),DecompressionArguments(Compression::GZIP));

		if (!p.ChildExitedAndAllPipesClosed() && p.Can_Write()){
			p.QueueWrite(compressedCleartext);
			p.QueueWriteClose();
		}
		while(!p.ChildExitedAndAllPipesClosed()){
			if (p.Can_Read1()) retrievedCleartext+=p.Read1();
			if (p.Can_Read2()) p.Read2();
		}
		ASSERT_EQ(p.ExitStatus(),0);
	}

	ASSERT_TRUE(cleartext==retrievedCleartext);
}

TEST(XmltarTest,CompressHex)
{
	TransformHex transformHex;
	for(int i=0; i<500; ++i){
		std::string cleartext(i,'0');
		//std::cerr << "\"" << cleartext << "\"" << std::endl;
		std::string compressedtext=transformHex.CompressString(cleartext);
		//std::cerr << "\"" << compressedtext << "\"" << std::endl;
		std::cerr << transformHex.MaximumCompressedtextSizeGivenPlaintextSize(cleartext.size()) << " " << compressedtext.size() << std::endl;

		ASSERT_TRUE(transformHex.MaximumCompressedtextSizeGivenPlaintextSize(cleartext.size())==compressedtext.size());
	}
}
