/*
 * Compressor.hpp
 *
 *  Created on: Oct 19, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_COMPRESSORS_COMPRESSOR_HPP_
#define SRC_COMMON_COMPRESSORS_COMPRESSOR_HPP_

#include <string>

#include "Generated/Utilities/Identity.hpp"
#include "Generated/Utilities/Hex.hpp"
#include "Generated/Utilities/Gzip.hpp"
#include "Generated/Utilities/ZstdCompress.hpp"
#include "Generated/Utilities/Sha3.hpp"

class CompressorInterface {
	std::string name_;
public:
	CompressorInterface(std::string const & name) : name_(name) {}
	virtual ~CompressorInterface(){}

	std::string CompressorName(){ return name_; }

	virtual void Open()=0;
	virtual std::string ForceWrite(std::string const & s)=0;
	virtual std::string ForceWriteAndClose(std::string const & s)=0;
	virtual std::string OpenForceWriteAndClose(std::string const & s)=0;

	std::string Name(){ return name_; }
	virtual CompressorInterface *clone()=0;

	virtual size_t ReadCount()=0;
	virtual size_t WriteCount()=0;

	virtual std::streamoff MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize)=0;
	virtual std::streamoff MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize)=0;
};

template<typename T> class Compressor : public CompressorInterface {
	T compressor_;

	size_t readCount_;
	size_t writeCount_;
public:
	Compressor(std::string const & name) : CompressorInterface(name) {}
	virtual ~Compressor() {}

	void Open() override {
		compressor_.Open();
		readCount_=0;
		writeCount_=0;
	}

	std::string ForceWrite(std::string const & s) override {
		std::string result(compressor_.ForceWrite(s));
		writeCount_+=s.size();
		readCount_+=result.size();

		return result;
	}

	std::string ForceWriteAndClose(std::string const & s) override {
		std::string result(compressor_.ForceWriteAndClose(s));
		writeCount_+=s.size();
		readCount_+=result.size();

		return result;
	}

	std::string OpenForceWriteAndClose(std::string const & s) override {
		std::string result(compressor_.OpenForceWriteAndClose(s));
		writeCount_+=s.size();
		readCount_+=result.size();

		return result;
	}

	CompressorInterface *clone() override {
		return new Compressor<T>(Name());
	}

	size_t ReadCount() override { return readCount_; }
	size_t WriteCount() override { return writeCount_; }

	std::streamoff MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){
		return compressor_.MaximumCompressedtextSizeGivenPlaintextSize(plaintextSize);
	}

	std::streamoff MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){
		return compressor_.MinimumPlaintextSizeGivenCompressedtextSize(compressedtextSize);
	}
};

#if 0
template<>
std::streamoff Compressor<Identity>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize);
template<>
std::streamoff Compressor<Identity>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize);
template<>
std::string Compressor<Identity>::MinimumCompressionString();

template<>
std::streamoff Compressor<HexEncode>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize);
template<>
std::streamoff Compressor<HexEncode>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize);
template<>
std::string Compressor<HexEncode>::MinimumCompressionString();

template<>
std::streamoff Compressor<HexDecode>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize);
template<>
std::streamoff Compressor<HexDecode>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize);
template<>
std::string Compressor<HexDecode>::MinimumCompressionString();

template<>
std::streamoff Compressor<Zlib::Gzip>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize);
template<>
std::streamoff Compressor<Zlib::Gzip>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize);
template<>
std::string Compressor<Zlib::Gzip>::MinimumCompressionString();

template<>
std::streamoff Compressor<Zlib::GzipRaw>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize);
template<>
std::streamoff Compressor<Zlib::GzipRaw>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize);
template<>
std::string Compressor<Zlib::GzipRaw>::MinimumCompressionString();

template<>
std::streamoff Compressor<Zlib::Gunzip>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize);
template<>
std::streamoff Compressor<Zlib::Gunzip>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize);
template<>
std::string Compressor<Zlib::Gunzip>::MinimumCompressionString();

template<>
std::streamoff Compressor<ZstdCompress>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize);
template<>
std::streamoff Compressor<ZstdCompress>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize);
template<>
std::string Compressor<ZstdCompress>::MinimumCompressionString();

template<>
std::streamoff Compressor<ZstdCompressRaw>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize);
template<>
std::streamoff Compressor<ZstdCompressRaw>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize);
template<>
std::string Compressor<ZstdCompressRaw>::MinimumCompressionString();

template<>
std::streamoff Compressor<ZstdDecompress>::MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize);
template<>
std::streamoff Compressor<ZstdDecompress>::MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize);
template<>
std::string Compressor<ZstdDecompress>::MinimumCompressionString();
#endif

#endif /* SRC_COMMON_COMPRESSORS_COMPRESSOR_HPP_ */
