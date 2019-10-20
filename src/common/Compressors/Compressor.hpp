/*
 * Compressor.hpp
 *
 *  Created on: Oct 19, 2019
 *      Author: dbetz
 */

#include <string>

#include "Generated/Utilities/Identity.hpp"
#include "Generated/Utilities/Hex.hpp"
#include "Generated/Utilities/Zlib.hpp"
#include "Generated/Utilities/Zstd.hpp"

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

	virtual CompressorInterface *clone();
};

template<typename T> class Compressor : public CompressorInterface {
	T compressor_;
public:
	Compressor(std::string const & name) : CompressorInterface(name) {}
	virtual ~Compressor() {}

	void Open() override {
		compressor_.Open();
	}

	std::string ForceWrite(std::string const & s) override {
		return compressor_.ForceWrite(s);
	}

	std::string ForceWriteAndClose(std::string const & s) override {
		return compressor_.ForceWriteAndClose(s);
	}

	std::string OpenForceWriteAndClose(std::string const & s) override {
		return compressor_.OpenForceWriteAndClose(s);
	}

	CompressorInterface *clone(std::string const & s){
		return new T(s);
	}
};
