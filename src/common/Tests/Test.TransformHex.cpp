/*
 * Test.Compression.cpp
 *
 *  Created on: Nov 23, 2017
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Generated/Process/Utilities.hpp"
#include "Transform/TransformHex.hpp"

std::string RepeatString(std::string s, int count){
	std::string result;

	result.reserve(s.size()*count);
	while(count-->0)
		result+=s;

	return result;
}

TEST(TransformHex,Compress_0_Characters)
{
	TransformHex transformHex("CompressHex");
	transformHex.OpenCompression();
	std::string plainText="";
	std::string expectedCompressedText="\n";
	std::string compressedText=transformHex.ForceWriteAndClose(plainText);

	EXPECT_EQ(compressedText, expectedCompressedText);
	EXPECT_EQ(compressedText.size(), transformHex.MaximumCompressedtextSizeGivenPlaintextSize(plainText.size()));
	EXPECT_EQ(plainText.size(), transformHex.MinimumPlaintextSizeGivenCompressedtextSize(compressedText.size()));
}

TEST(TransformHex,Compress_29_Characters)
{
	TransformHex transformHex("CompressHex");
	transformHex.OpenCompression();
	std::string plainText(29,'a');
	std::string expectedCompressedText=RepeatString("61",29)+"\n";
	std::string compressedText=transformHex.ForceWriteAndClose(plainText);

	EXPECT_EQ(compressedText, expectedCompressedText);
	EXPECT_EQ(compressedText.size(), transformHex.MaximumCompressedtextSizeGivenPlaintextSize(plainText.size()));
	EXPECT_EQ(plainText.size(), transformHex.MinimumPlaintextSizeGivenCompressedtextSize(compressedText.size()));
}

TEST(TransformHex,Compress_30_Characters)
{
	TransformHex transformHex("CompressHex");
	transformHex.OpenCompression();
	std::string plainText(30,'a');
	std::string expectedCompressedText=RepeatString("61",30)+"\n";
	std::string compressedText=transformHex.ForceWriteAndClose(plainText);

	EXPECT_EQ(compressedText, expectedCompressedText);
	EXPECT_EQ(compressedText.size(), transformHex.MaximumCompressedtextSizeGivenPlaintextSize(plainText.size()));
	EXPECT_EQ(plainText.size(), transformHex.MinimumPlaintextSizeGivenCompressedtextSize(compressedText.size()));
}

TEST(TransformHex,Compress_31_Characters)
{
	TransformHex transformHex("CompressHex");
	transformHex.OpenCompression();
	std::string plainText(31,'a');
	std::string expectedCompressedText=RepeatString("61",30)+"\n61\n";
	std::string compressedText=transformHex.ForceWriteAndClose(plainText);

	EXPECT_EQ(compressedText, expectedCompressedText);
	EXPECT_EQ(compressedText.size(), transformHex.MaximumCompressedtextSizeGivenPlaintextSize(plainText.size()));
	EXPECT_EQ(plainText.size(), transformHex.MinimumPlaintextSizeGivenCompressedtextSize(compressedText.size()));
}
