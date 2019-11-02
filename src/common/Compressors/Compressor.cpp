/*
 * Compressor.cpp
 *
 *  Created on: Oct 26, 2019
 *      Author: dbetz
 */

#include "Compressors/Compressor.hpp"

#if 0
template<>
std::streamoff Compressor<Identity>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){
	return plaintextSize;
}
template<>
std::streamoff Compressor<Identity>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){
	return compressedtextSize;
}
template<>
std::string Compressor<Identity>::MinimumCompressionString(){}

template<>
std::streamoff Compressor<HexEncode>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){}
template<>
std::streamoff Compressor<HexEncode>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){}
template<>
std::string Compressor<HexEncode>::MinimumCompressionString(){}

template<>
std::streamoff Compressor<HexDecode>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){}
template<>
std::streamoff Compressor<HexDecode>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){}
template<>
std::string Compressor<HexDecode>::MinimumCompressionString(){}

template<>
std::streamoff Compressor<Zlib::Gzip>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){}
template<>
std::streamoff Compressor<Zlib::Gzip>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){}
template<>
std::string Compressor<Zlib::Gzip>::MinimumCompressionString(){}

template<>
std::streamoff Compressor<Zlib::GzipRaw>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){}
template<>
std::streamoff Compressor<Zlib::GzipRaw>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){}
template<>
std::string Compressor<Zlib::GzipRaw>::MinimumCompressionString(){}

template<>
std::streamoff Compressor<Zlib::Gunzip>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){}
template<>
std::streamoff Compressor<Zlib::Gunzip>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){}
template<>
std::string Compressor<Zlib::Gunzip>::MinimumCompressionString(){}

template<>
std::streamoff Compressor<ZstdCompress>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){}
template<>
std::streamoff Compressor<ZstdCompress>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){}
template<>
std::string Compressor<ZstdCompress>::MinimumCompressionString(){}

template<>
std::streamoff Compressor<ZstdCompressRaw>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){}
template<>
std::streamoff Compressor<ZstdCompressRaw>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){}
template<>
std::string Compressor<ZstdCompressRaw>::MinimumCompressionString(){}

template<>
std::streamoff Compressor<ZstdDecompress>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){}
template<>
std::streamoff Compressor<ZstdDecompress>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){}
template<>
std::string Compressor<ZstdDecompress>::MinimumCompressionString(){}
#endif
