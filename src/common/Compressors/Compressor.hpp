/*
 * Compressor.hpp
 *
 *  Created on: Oct 19, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_COMPRESSORS_COMPRESSOR_HPP_
#define SRC_COMMON_COMPRESSORS_COMPRESSOR_HPP_

#include <string>
#include <iostream>

#include "Generated/Utilities/Identity.hpp"
#include "Generated/Utilities/Hex.hpp"
#include "Generated/Utilities/Gzip.hpp"
#include "Generated/Utilities/ZstdCompress.hpp"
#include "Generated/Utilities/Sha3.hpp"
#include "Generated/Utilities/Debug2.hpp"

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

	virtual std::string GenerateCompressedFile(std::string & start, std::string finish, size_t size)=0;
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

#if 0
	std::streamoff MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){
		return compressor_.MinimumPlaintextSizeGivenCompressedtextSize(compressedtextSize);
	}
#endif
	std::streamoff MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){	//FIXME - duplicated in Transform.cpp
		betz::Debug2 dbg("TransformProcess::MinimumPlaintextSizeGivenCompressedtextSize");
		std::cerr << dbg << ": compressedtextSize=" << compressedtextSize << std::endl;
		std::streamoff 	plaintextSizeLB=0,
						plaintextSizeUB=compressedtextSize;

		if (compressedtextSize<MaximumCompressedtextSizeGivenPlaintextSize(0)){
			return 0;

			std::cerr << dbg << ": compressedtextSize=" << compressedtextSize << std::endl;
			std::cerr << dbg << ": MaximumCompressedtextSizeGivenPlaintextSize(0)=" << MaximumCompressedtextSizeGivenPlaintextSize(0) << std::endl;
			throw std::invalid_argument("Transform::MinimumPlaintextSizeGivenCompressedtextSize: compressedtextSize too small");
		}

		std::cerr << dbg << ": compressedtextSize=" << compressedtextSize << " < MaximumCompressedtextSizeGivenPlaintextSize(0)=" << MaximumCompressedtextSizeGivenPlaintextSize(0) << std::endl;
		std::cerr << dbg << ": plaintextSizeLB=" << plaintextSizeLB << " plaintextSizeUB=" << plaintextSizeUB << std::endl;

		std::cerr << dbg << ": 1" << std::endl;

		while(MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)<compressedtextSize){
			std::cerr << dbg << ":1: plaintextSizeLB=" << plaintextSizeLB << " plaintextSizeUB=" << plaintextSizeUB << std::endl;
			plaintextSizeUB+=compressedtextSize;
		}

		//std::cerr << dbg << ": 2" << std::endl;

		while(plaintextSizeUB-plaintextSizeLB>1){
			std::cerr << dbg << ":2: plaintextSizeLB=" << plaintextSizeLB << " plaintextSizeUB=" << plaintextSizeUB << std::endl;

			// mid is always strictly less than plaintextSizeUB

			size_t mid=(plaintextSizeLB+plaintextSizeUB)/2;

			// plaintextSizeUB points to somewhere in the range of possible plaintext sizes

			if (MaximumCompressedtextSizeGivenPlaintextSize(mid)>=compressedtextSize)
				plaintextSizeUB=mid;

			// plaintextSizeLB points below the range of possible plaintext sizes

			else // if (MaximumCompressedtextSizeGivenPlaintextSize(mid)<compressedtextSize)
				plaintextSizeLB=mid;
		}

		std::cerr << dbg << ": compressedtextSize=" << compressedtextSize << std::endl;
		std::cerr << dbg << ": plaintextSizeUB=" << plaintextSizeUB << std::endl;
		std::cerr << dbg << ": MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)=" << MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB) << std::endl;

		std::streamoff result;

		if (MaximumCompressedtextSizeGivenPlaintextSize(plaintextSizeUB)<compressedtextSize)
			result=plaintextSizeUB;
		else
			result=plaintextSizeLB;

		return result;
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
