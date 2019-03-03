/*
 * Test.Compression.cpp
 *
 *  Created on: Nov 23, 2017
 *      Author: dbetz
 */

#include <boost/filesystem.hpp>

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
	std::string expectedCompressedText;
	if (std::string(transformHex.CompressionName())=="xxd")
		expectedCompressedText="";
	else
		expectedCompressedText="\n";
	std::string compressedText=transformHex.ForceWriteAndClose(plainText);

	EXPECT_EQ(compressedText, expectedCompressedText);
	EXPECT_EQ(compressedText.size(), transformHex.MaximumCompressedtextSizeGivenPlaintextSize(plainText.size()));
	EXPECT_EQ(plainText.size(), transformHex.MinimumPlaintextSizeGivenCompressedtextSize(compressedText.size()));

	TransformHex deTransformHex("DecompressHex");
	deTransformHex.OpenDecompression();
	std::string revertedPlainText=deTransformHex.ForceWriteAndClose(expectedCompressedText);

	EXPECT_EQ(revertedPlainText,plainText);
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

	TransformHex deTransformHex("DecompressHex");
	deTransformHex.OpenDecompression();
	std::string revertedPlainText=deTransformHex.ForceWriteAndClose(expectedCompressedText);

	EXPECT_EQ(revertedPlainText,plainText);
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

	TransformHex deTransformHex("DecompressHex");
	deTransformHex.OpenDecompression();
	std::string revertedPlainText=deTransformHex.ForceWriteAndClose(expectedCompressedText);

	EXPECT_EQ(revertedPlainText,plainText);
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

	TransformHex deTransformHex("DecompressHex");
	deTransformHex.OpenDecompression();
	std::string revertedPlainText=deTransformHex.ForceWriteAndClose(expectedCompressedText);

	EXPECT_EQ(revertedPlainText,plainText);
}

TEST(TransformHex,Compress_Many_Files)
{
	for(auto & p : boost::filesystem::directory_iterator("/usr/bin")){
		//if (boost::filesystem::file_size(p)>1000) continue;
		std::ifstream ifs(p.path().c_str());

		if (ifs.good()){
			TransformHex transformHex("CompressHex");
			TransformHex unTransformHex("CompressHex");
			transformHex.OpenCompression();
			unTransformHex.OpenDecompression();

			std::string plainText;
			std::string transformedText;
			std::string revertedPlainText;

			plainText.reserve(boost::filesystem::file_size(p));
			transformedText.reserve(boost::filesystem::file_size(p));
			revertedPlainText.reserve(boost::filesystem::file_size(p));

			std::string tmp;
			char buf[1024];
			for( ; ifs; ){
				ifs.read(buf,sizeof(buf));
				plainText+=std::string(buf,ifs.gcount());
				tmp=transformHex.ForceWrite(std::string(buf,ifs.gcount()));
				transformedText+=tmp;
				revertedPlainText+=unTransformHex.ForceWrite(tmp);
			}

			tmp=transformHex.ForceWriteAndClose("");
			transformedText+=tmp;
			revertedPlainText+=unTransformHex.ForceWriteAndClose(tmp);
			std::cerr << "************************* " << p.path().string() << " " << transformedText.size() << std::endl;

			EXPECT_EQ(plainText, revertedPlainText);
			EXPECT_EQ(plainText.size(), transformHex.WriteCount());
			EXPECT_EQ(transformedText.size(), transformHex.ReadCount());
			EXPECT_EQ(transformedText.size(), unTransformHex.WriteCount());
			EXPECT_EQ(plainText.size(), unTransformHex.ReadCount());
			EXPECT_EQ(transformHex.ReadCount(), transformHex.MaximumCompressedtextSizeGivenPlaintextSize(plainText.size()));
			EXPECT_EQ(plainText.size(), transformHex.MinimumPlaintextSizeGivenCompressedtextSize(transformedText.size()));
		}
	}

	std::ostringstream osss;
	TransformHex transformHex("CompressHex");
	transformHex.OpenCompression();
	std::string plainText(31,'a');
	std::string expectedCompressedText=RepeatString("61",30)+"\n61\n";
	std::string compressedText=transformHex.ForceWriteAndClose(plainText);

	EXPECT_EQ(compressedText, expectedCompressedText);
	EXPECT_EQ(compressedText.size(), transformHex.MaximumCompressedtextSizeGivenPlaintextSize(plainText.size()));
	EXPECT_EQ(plainText.size(), transformHex.MinimumPlaintextSizeGivenCompressedtextSize(compressedText.size()));
}
