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

#ifndef Pipe_Codecs_hpp_
#define Pipe_Codecs_hpp_

#include "basic_pipe_transitbuf.hpp"

class identity_encoder_transitbuf : public pipe_transitbuf_base {
public:
    identity_encoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/bin/cat", "cat");
    }

    size_t encoded_length(size_t n){
        DEBUGCXX(debugcxx,"identity_encoder_transitbuf::encoded_length");

        size_t result;

        if (pipe.get_state()==Bidirectional_Pipe::UNOPENED)
            result=null_encoded_length(n);
        else if (pipe.get_state()==Bidirectional_Pipe::EXITED)
            result=pipe.Read1_Count();
        else
            result=null_encoded_length(n+pipe.Write_Count());

        return result;
    }

    size_t null_encoded_length(size_t n){
        return n;
    }
};

class identity_decoder_transitbuf : public pipe_transitbuf_base {
public:
    identity_decoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/bin/cat", "cat");
    }

    size_t encoded_length(size_t n){
        throw "identity_decoder_transitbuf::encoded_length: not implemented";
    }

    size_t null_encoded_length(size_t n){
        throw "identity_decoder_transitbuf::null_encoded_length: not implemented";
    }
};

class base16_encoder_transitbuf : public pipe_transitbuf_base {
public:
    base16_encoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/hexdump", "hexdump", "-v", "-e", "1/1 \"%02x\"");
    }

    size_t encoded_length(size_t n){
        DEBUGCXX(debugcxx,"base16_encoder_transitbuf::encoded_length");

        size_t result;

        if (pipe.get_state()==Bidirectional_Pipe::UNOPENED)
            result=null_encoded_length(n);
        else if (pipe.get_state()==Bidirectional_Pipe::EXITED)
            result=pipe.Read1_Count();
        else
            result=null_encoded_length(n+pipe.Write_Count());

        return result;
    }

    size_t null_encoded_length(size_t n){
        return 2*n;
    }
};

class base16_decoder_transitbuf : public pipe_transitbuf_base {
public:
    base16_decoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/xxd", "xxd","-r","-ps");
    }

    size_t encoded_length(size_t n){
        throw "base16_decoder_transitbuf::encoded_length: not implemented";
    }

    size_t null_encoded_length(size_t n){
        throw "base16_decoder_transitbuf::null_encoded_length: not implemented";
    }
};

class base64_encoder_transitbuf : public pipe_transitbuf_base {
public:
    base64_encoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/base64", "base64", "-w", "0");
    }

    size_t encoded_length(size_t n){
        DEBUGCXX(debugcxx,"base64_encoder_transitbuf::encoded_length");

        size_t result;

        if (pipe.get_state()==Bidirectional_Pipe::UNOPENED)
            result=null_encoded_length(n);
        else if (pipe.get_state()==Bidirectional_Pipe::EXITED)
            result=pipe.Read1_Count();
        else
            result=null_encoded_length(n+pipe.Write_Count());

        return result;
    }

    size_t null_encoded_length(size_t n){
        return 4*((n+2)/3);
    }
};

class base64_decoder_transitbuf : public pipe_transitbuf_base {
public:
    base64_decoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/base64", "base64","-d","-w","0");
    }

    size_t encoded_length(size_t n){
        throw "base64_decoder_transitbuf::encoded_length: not implemented";
    }

    size_t null_encoded_length(size_t n){
        throw "base64_decoder_transitbuf::null_encoded_length: not implemented";
    }
};

class gzip_encoder_transitbuf : public pipe_transitbuf_base {
public:
    gzip_encoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/gzip", "gzip","-fc");
    }

    size_t encoded_length(size_t n){
        DEBUGCXX(debugcxx,"gzip_encoder_transitbuf::encoded_length");

        size_t result;

        if (pipe.get_state()==Bidirectional_Pipe::UNOPENED)
            result=null_encoded_length(n);
        else if (pipe.get_state()==Bidirectional_Pipe::EXITED)
            result=pipe.Read1_Count();
        else
            result=null_encoded_length(n+pipe.Write_Count());

        return result;
    }

    size_t null_encoded_length(size_t n){
        return n+((n+7)>>3)+((n+63)>>6)+11+18;
    }
};

class gzip_decoder_transitbuf : public pipe_transitbuf_base {
public:
    gzip_decoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/gzip", "gzip","-fdc");
    }

    size_t encoded_length(size_t n){
        throw "gzip_decoder_transitbuf::encoded_length: not implemented";
    }

    size_t null_encoded_length(size_t n){
        throw "gzip_decoder_transitbuf::null_encoded_length: not implemented";
    }
};

class bzip2_encoder_transitbuf : public pipe_transitbuf_base {
public:
    bzip2_encoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/bzip2", "bzip2","-fc");
    }

    size_t encoded_length(size_t n){
        DEBUGCXX(debugcxx,"bzip2_encoder_transitbuf::encoded_length");

        size_t result;

        if (pipe.get_state()==Bidirectional_Pipe::UNOPENED)
            result=null_encoded_length(n);
        else if (pipe.get_state()==Bidirectional_Pipe::EXITED)
            result=pipe.Read1_Count();
        else
            result=null_encoded_length(n+pipe.Write_Count());

        return result;
    }

    size_t null_encoded_length(size_t n){
        return n+((n+7)>>3)+((n+63)>>6)+11+18;
    }
};

class bzip2_decoder_transitbuf : public pipe_transitbuf_base {
public:
    bzip2_decoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/bzip2", "bzip2","-fdc");
    }

    size_t encoded_length(size_t n){
        throw "bzip2_decoder_transitbuf::encoded_length: not implemented";
    }

    size_t null_encoded_length(size_t n){
        throw "bzip2_decoder_transitbuf::null_encoded_length: not implemented";
    }
};

class xz_encoder_transitbuf : public pipe_transitbuf_base {
public:
    xz_encoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/xz", "xz","-fc");
    }

    size_t encoded_length(size_t n){
        DEBUGCXX(debugcxx,"xz_encoder_transitbuf::encoded_length");

        size_t result;

        if (pipe.get_state()==Bidirectional_Pipe::UNOPENED)
            result=null_encoded_length(n);
        else if (pipe.get_state()==Bidirectional_Pipe::EXITED)
            result=pipe.Read1_Count();
        else
            result=null_encoded_length(n+pipe.Write_Count());

        return result;
    }

    size_t null_encoded_length(size_t n){
        return 63+n+(n>>14);
    }
};

class xz_decoder_transitbuf : public pipe_transitbuf_base {
public:
    xz_decoder_transitbuf(void){
    }

    transitbuf_base*
    open(std::ios_base::openmode open_mode,
            __streambuf_type *sbuf, const char *prog=NULL, const char *arg0=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL){
        pipe_transitbuf_base::open(open_mode, sbuf, "/usr/bin/xz", "xz","-fdc");
    }

    size_t encoded_length(size_t n){
        throw "xz_decoder_transitbuf::encoded_length: not implemented";
    }

    size_t null_encoded_length(size_t n){
        throw "xz_decoder_transitbuf::null_encoded_length: not implemented";
    }
};

#endif /* Pipe_Codecs_hpp_ */
