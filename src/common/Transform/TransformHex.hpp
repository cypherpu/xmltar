/*
 * TransformHex.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_
#define SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_

#include "Transform/TransformProcess.hpp"

static char conversion[]={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

class TransformHex : public Transform {
public:
	std::string data_;
	size_t read_count;
	size_t write_count;
	size_t read_count_mod30_;

	std::string HexEncode(std::string const & s){
		std::string result;
		result.reserve(2*s.size());

		for(auto i : s){
			unsigned char c=(unsigned char) i;
			++read_count_mod30_;

			result.push_back(conversion[(c & 0xF0)>>4]);
			result.push_back(conversion[c & 0x0F]);

			if (read_count_mod30_==30){
				result+='\n';
				read_count_mod30_=0;
			}
		}

		return result;
	}

	std::string ActualCompressorVersionString() override { return "hex"; }
	std::string ExpectedCompressorVersionString() override { return "hex"; }
	bool CorrectCompressorVersion() override { return true; }
	std::string HeaderMagicNumber(std::string identity) override { return ""; }
	static std::string StaticHeaderMagicNumber(std::string identity){ return identity; }
	std::string TrailerMagicNumber() override { return ""; }
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize) override {
		if (plaintextSize==0) return 1;
		else return 2*plaintextSize+(plaintextSize+29)/30;
	}
	/*
	 * newlines:           1  1..1   2..2     3..3
	 * plainTextSize:      0  1..60  61..120  121..180
	 * compressedTextSize: 1  1..61  63..122  124..183
	 */
	size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedTextSize) override {
		if (compressedTextSize<=1) return 0;
		return (compressedTextSize-(compressedTextSize+60)/61)/2;
	}
	char const *CompressionCommand() override { return "hex"; }
	char const *CompressionName() override { return "hex"; }
	std::vector<char const *> CompressionArguments() override { return std::vector<char const *>(); }
	std::vector<char const *> DecompressionArguments() override { return std::vector<char const *>(); }
	std::vector<char const *> VersionArguments() override { return std::vector<char const *>(); }
	size_t EmptyCompressedSize() override { return 0; }
	size_t MinimumUsableCompressedSize() override { return 0; }
	std::string MinimumCompressionString() override { return ""; }
	std::string CompressString(std::string const & s) override { return s; }
	std::string DecompressString(std::string const & s) override { return s; }
	Transform *clone(){
		return new TransformHex(name());
	}

	void OpenCompression() override { if (data_!="") throw std::logic_error("TransformIdentity::OpenCompression: data_ not empty"); }
	void OpenDecompression() override { if (data_!="") throw std::logic_error("TransformIdentity::OpenDecompression: data_ not empty"); }
	std::string ForceWrite(std::string data) override {
		std::string tmp=HexEncode(data);
		write_count+=data.size();
		read_count+=tmp.size();
		return tmp;
	}
	std::string ForceWriteAndClose(std::string input) override {
		std::string tmp=ForceWrite(input);
		if (read_count_mod30_>0 || write_count==0){
			tmp+="\n";
			read_count++;
		}
		return tmp;
	}
	size_t ReadCount() override { return read_count; }
	size_t WriteCount() override { return write_count; }

	TransformHex(std::string const & name)
		: Transform(name), read_count(0), write_count(0), read_count_mod30_(0) {}
	~TransformHex(){}
};

#if 0

class TransformHex  : public TransformProcess {
public:
	// std::string ActualCompressorVersionString();
	std::string ExpectedCompressorVersionString() override { return "xxd V1.10 27oct98 by Juergen Weigert"; }
	// std::string CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity) override { return ""; }
	std::string TrailerMagicNumber() override { return ""; }
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize) override {
		// return 1+(2*plaintextSize-1)/60+2*plaintextSize;
		return (60+2*plaintextSize-1)/60+2*plaintextSize;
	}
	// size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	char const *CompressionCommand() override { return "/usr/bin/xxd"; }
	char const *CompressionName() override { return "xxd"; }
	std::vector<char const *> CompressionArguments() override { return std::vector<char const *>({"xxd","-ps"}); }
	std::vector<char const *> DecompressionArguments() override { return std::vector<char const *>({"xxd","-ps","-r"}); }
	std::vector<char const *> VersionArguments() override { return std::vector<char const *>({"xxd","-v"}); }
	size_t EmptyCompressedSize() override { throw std::logic_error("TransformHex::EmptyArchiveSize: should not be called"); }
	size_t MinimumUsableCompressedSize() override { throw std::logic_error("TransformHex::EmptyArchiveSize: should not be called"); }
	std::string MinimumCompressionString() override { return ""; }
	// std::string CompressString(std::string const & s);
	// std::string DecompressString(std::string const & s);
	Transform *clone() override { return new TransformHex(name()); }

	TransformHex(std::string const & name)
		: TransformProcess(name) {}
	~TransformHex(){
		std::cerr << "Calling TransformHex::~TransformHex" << std::endl;
		if (a_.readState()!=Descriptor::CLOSED)
			std::cerr << "TransformHex::~TransformHex: opened read state a_" << name() << std::endl;
		if (a_.writeState()!=Descriptor::CLOSED)
			std::cerr << "TransformHex::~TransformHex: opened write state a_" << name() << std::endl;
		if (b_.readState()!=Descriptor::CLOSED)
			std::cerr << "TransformHex::~TransformHex: opened read state b_" << name() << std::endl;
		if (b_.writeState()!=Descriptor::CLOSED)
			std::cerr << "TransformHex::~TransformHex: opened write state b_" << name() << std::endl;
	}

};

#endif

#if 0
class TransformHex  : public TransformProcess {
public:
	// std::string ActualCompressorVersionString();
	std::string ExpectedCompressorVersionString() override { return "hexdump from util-linux 2.32.1"; }
	// std::string CorrectCompressorVersion();
	std::string HeaderMagicNumber(std::string identity) override { return ""; }
	std::string TrailerMagicNumber() override { return ""; }
	size_t MaximumCompressedtextSizeGivenPlaintextSize(size_t plaintextSize) override {
		// return 1+(2*plaintextSize-1)/60+2*plaintextSize;
		return (60+2*plaintextSize-1)/60+2*plaintextSize;
	}
	// size_t MinimumPlaintextSizeGivenCompressedtextSize(size_t compressedtextSize);
	char const *CompressionCommand() override { return "/usr/bin/hexdump"; }
	char const *CompressionName() override { return "hexdump"; }
	std::vector<char const *> CompressionArguments() override { return std::vector<char const *>({"hexdump","-e","'30/1 \"%02x\" \"\n\"'"}); }
	std::vector<char const *> DecompressionArguments() override { return std::vector<char const *>({"xxd","-ps","-r"}); }
	std::vector<char const *> VersionArguments() override { return std::vector<char const *>({"hexdump","--version"}); }
	size_t EmptyCompressedSize() override { throw std::logic_error("TransformHex::EmptyArchiveSize: should not be called"); }
	size_t MinimumUsableCompressedSize() override { throw std::logic_error("TransformHex::EmptyArchiveSize: should not be called"); }
	std::string MinimumCompressionString() override { return ""; }
	// std::string CompressString(std::string const & s);
	// std::string DecompressString(std::string const & s);
	Transform *clone() override { return new TransformHex(name()); }

	TransformHex(std::string const & name)
		: TransformProcess(name) {}
	~TransformHex(){
		std::cerr << "Calling TransformHex::~TransformHex" << std::endl;
		if (a_.readState()!=Descriptor::CLOSED)
			std::cerr << "TransformHex::~TransformHex: opened read state a_" << name() << std::endl;
		if (a_.writeState()!=Descriptor::CLOSED)
			std::cerr << "TransformHex::~TransformHex: opened write state a_" << name() << std::endl;
		if (b_.readState()!=Descriptor::CLOSED)
			std::cerr << "TransformHex::~TransformHex: opened read state b_" << name() << std::endl;
		if (b_.writeState()!=Descriptor::CLOSED)
			std::cerr << "TransformHex::~TransformHex: opened write state b_" << name() << std::endl;
	}

};

#endif

#endif /* SRC_COMMON_TRANSFORM_TRANSFORMHEX_HPP_ */
