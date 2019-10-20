/*
 * Compressor.hpp
 *
 *  Created on: Oct 19, 2019
 *      Author: dbetz
 */

#include <string>

class CompressorInterface {
public:
	CompressorInterface(){}
	virtual ~CompressorInterface(){}

	virtual void Open()=0;
	virtual std::string ForceWrite(std::string const & s)=0;
	virtual std::string ForceWriteAndClose(std::string const & s)=0;
	virtual std::string OpenForceWriteAndClose(std::string const & s)=0;
};

template<typename T> class Compressor : public CompressorInterface {
	T compressor_;
public:
	Compressor() {}
	virtual ~Compressor() {}

	void Open() override {
		compressor_.Open();
	}

	std::string ForceWrite(std::string const s) override {
		return compressor_.ForceWrite(s);
	}

	std::string ForceWriteAndClose(std::string const s) override {
		return compressor_.ForceWriteAndClose(s);
	}

	std::string OpenForceWriteAndClose(std::string const s) override {
		return compressor_.OpenForceWriteAndClose(s);
	}
};
