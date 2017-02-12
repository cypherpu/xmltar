/*

Copyright 2010 by David A. Betz

This file is part of xmltar.

xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xmltar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef XmltarOptions_hpp_

#define XmltarOptions_hpp_

#include <limits>

#include "Pipe_Codecs.hpp"

class XmltarOptions {
public:
    enum Operation { NOOP, APPEND, CREATE, LIST, EXTRACT };
    enum Compression { IDENTITY, GZIP, BZIP2, XZ };
	enum Encoding { BASE16, BASE64 };

protected:
	transitbuf_base *New_Compressor(XmltarOptions::Compression comp){
		transitbuf_base *tb=0;

		switch(comp){
		case XmltarOptions::IDENTITY:
			tb=new identity_encoder_transitbuf;
			break;
		case XmltarOptions::GZIP:
			tb=new gzip_encoder_transitbuf;
			break;
		case XmltarOptions::BZIP2:
			tb=new bzip2_encoder_transitbuf;
			break;
		case XmltarOptions::XZ:
			tb=new xz_encoder_transitbuf;
			break;
		default:
			throw "XmltarOptions::New_Precompressor: unrecognized precompress";
		}

		return tb;
	}

	transitbuf_base *New_Decompressor(XmltarOptions::Compression comp){
		transitbuf_base *tb=0;

		switch(comp){
		case XmltarOptions::IDENTITY:
			tb=new identity_decoder_transitbuf;
			break;
		case XmltarOptions::GZIP:
			tb=new gzip_decoder_transitbuf;
			break;
		case XmltarOptions::BZIP2:
			tb=new bzip2_decoder_transitbuf;
			break;
		case XmltarOptions::XZ:
			tb=new xz_decoder_transitbuf;
			break;
		default:
			throw "XmltarOptions::New_Decompressor: unknown postcompressor type";
		}

		return tb;
	}

public:
	Operation operation;
	int verbosity;
	bool multi_volume;
	Compression precompress;
	Compression postcompress;
	Encoding encoding;
	bool incremental;
	bool compress_listed_incremental_file;
	size_t tape_length;
	size_t stop_after;
	std::vector<boost::filesystem::path> source_files;
	boost::filesystem::path listed_incremental_file;
	boost::filesystem::path files_from;
	std::vector<boost::filesystem::path> exclude_files;
	std::string tag;
   	bool tabs_;
   	bool newlines_;

	std::string base_xmltar_file_name;							// xmltar_file;
	unsigned int starting_sequence_number;						// starting_volume;

	std::string current_xmltar_file_name;
	unsigned int current_sequence_number;

    XmltarOptions(void)
        : operation(NOOP), verbosity(0), multi_volume(false), precompress(IDENTITY), postcompress(IDENTITY),
          encoding(BASE16), incremental(false), compress_listed_incremental_file(false),
          tape_length(std::numeric_limits<std::size_t>::max()), stop_after(std::numeric_limits<std::size_t>::max()),
          tag(""), tabs_(true), newlines_(true) { }

    std::string HeaderMagicNumber(void){
		switch(postcompress){
		case IDENTITY:
			return std::string("<?xml");
		case GZIP:
			return std::string("\x1f\x8b");
		case BZIP2:
			return std::string("BZh");
		case XZ:
			return std::string("\xfd\x37zXZ\0",6);
		default:
			throw "XmltarOptions::HeaderMagicNumber: unrecognized Compression";
		}
	}

    std::string TrailerMagicNumber(void){
		switch(postcompress){
		case IDENTITY:
			return std::string("</members");
		case GZIP:
			return std::string("\x1f\x8b");
		case BZIP2:
			return std::string("BZh");
		case XZ:
			return std::string("\xfd\x37zXZ\0",6);
		default:
			throw "XmltarOptions::TrailerMagicNumber: unrecognized Compression";
		}
	}

	transitbuf_base *New_Encoder(void){
	    switch(encoding){
	    case XmltarOptions::BASE16:
            return new base16_encoder_transitbuf;
	    case XmltarOptions::BASE64:
	        return new base64_encoder_transitbuf;
	    default:
	        throw "XmltarOptions::New_Encoder: unrecognized encoder";
	    }
	}

	transitbuf_base *New_Decoder(void){
	    switch(encoding){
	    case XmltarOptions::BASE16:
            return new base16_decoder_transitbuf;
	    case XmltarOptions::BASE64:
	        return new base64_decoder_transitbuf;
	    default:
	        throw "XmltarOptions::New_Decoder: unrecognized encoder";
	    }
	}

	transitbuf_base *New_Precompressor(void){
		return New_Compressor(precompress);
	}

	transitbuf_base *New_Postcompressor(void){
		return New_Compressor(postcompress);
	}

	transitbuf_base *New_Predecompressor(void){
		return New_Decompressor(precompress);
	}

	transitbuf_base *New_Postdecompressor(void){
		return New_Decompressor(postcompress);
	}

	std::string Tabs(const char *tabStr){
		return tabs_?std::string(tabStr):std::string("");
	}

	std::string Newline(){
		return newlines_?std::string("\n"):std::string("");
	}
};

#endif
