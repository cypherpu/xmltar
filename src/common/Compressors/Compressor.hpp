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
public:
	CompressorInterface(){}
	virtual ~CompressorInterface(){}

	virtual std::string Open()=0;
	virtual std::string ForceWrite(std::string const & s)=0;
	virtual std::string ForceWriteAndClose(std::string const & s)=0;
	virtual std::string OpenForceWriteAndClose(std::string const & s)=0;

	virtual size_t ReadCount()=0;
	virtual size_t WriteCount()=0;
};

class CompressorGeneralInterface : public CompressorInterface {
public:
	CompressorGeneralInterface(){}
	virtual ~CompressorGeneralInterface(){}

	virtual std::streamoff MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize)=0;
	virtual std::streamoff MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize)=0;

	virtual std::string CompressorName()=0;
};

class CompressorRawInterface : public CompressorInterface {
public:
	CompressorRawInterface(){}
	virtual ~CompressorRawInterface(){}

	virtual std::streamoff MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize)=0;
	virtual std::streamoff MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize)=0;
	virtual std::string GenerateCompressedText(size_t desiredSize, std::string data)=0;
};

template<typename T> class Compressor : public CompressorGeneralInterface {
	T compressor_;

	size_t readCount_;
	size_t writeCount_;
public:
	Compressor() : readCount_(), writeCount_() {}
	virtual ~Compressor() {}

	[[nodiscard]] std::string Open() override {
		std::string tmp=compressor_.Open();
		readCount_=tmp.size();
		writeCount_=0;

		return tmp;
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

	std::string CompressorName(){ return compressor_.CompressorName(); }
};

template<typename T> class CompressorRaw : public CompressorRawInterface {
	T compressor_;

	size_t readCount_;
	size_t writeCount_;
public:
	CompressorRaw() : readCount_(), writeCount_() {}
	virtual ~CompressorRaw() {}

	[[nodiscard]] std::string Open() override {
		std::string tmp=compressor_.Open();
		readCount_=tmp.size();
		writeCount_=0;

		return tmp;
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

	size_t ReadCount() override { return readCount_; }
	size_t WriteCount() override { return writeCount_; }

	std::streamoff MaximumCompressedtextSizeGivenPlaintextSize(std::streamoff plaintextSize){
		return compressor_.MaximumCompressedtextSizeGivenPlaintextSize(plaintextSize);
	}

	std::streamoff MinimumPlaintextSizeGivenCompressedtextSize(std::streamoff compressedtextSize){
		return compressor_.MinimumPlaintextSizeGivenCompressedtextSize(compressedtextSize);
	}

	std::string GenerateCompressedText(size_t desiredSize, std::string data){
		return compressor_.GenerateCompressedText(desiredSize,data);
	}

	std::string CompressorName(){ return compressor_.CompressorName(); }
};

#endif /* SRC_COMMON_COMPRESSORS_COMPRESSOR_HPP_ */
