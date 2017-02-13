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

#ifndef basic_transitbuf_hpp_
#define basic_transitbuf_hpp_

#include <streambuf>
#include <iostream>
#include <boost/scoped_array.hpp>

#include <iostream>

/*
 * transitbufs are a kind of streambuf which do some sort of processing on
 * byte/char streams.
 *
 * Types of transitbufs currently envisioned:
 * 		codecvtbuf - conversion between a wchar stream and char stream
 *  	identitybuf - conversion between a char stream and its self
 * 		base16buf - conversion between a char stream and a base16 (i.e., ascii hex) char stream
 *  	base64buf - conversion between a char stream and a base64 char streamm
 *  	gzipbuf - conversion between a char stream and a gzip compressed version of the char stream
 *  	teebuf - identity conversion from a single stream to 2 other streams
 *
 *  Many of these proposed transitbufs are octet-oriented, and assume that
 *  the system's char is an octet (i.e., 8 bits).
 */

/*
 * Why not use codecvt with a specialization of mbstate_t for the conversion?
 * At least under GNU systems, mbstate_t is defined as an array int[6]. While
 * this is probably big enough to hold a pointer on most systems, this is a
 * presumption unsupported by the C++ standard.
 *
 * So we could create our own state type. In this case, however, it would
 * sometimes be desirable to know how much expansion of the input could occur
 * under worst-case circumstances. It would be natural to use the do_length()
 * function of codecvt. The do_length function has the interface:
 *
 * 		int do_length(stateT& state, const externT* from, const externT* from_end, size_t max) const;
 *
 * There are a couple of problems with this. First, stateT is modified as if
 * a call to do_in/do_out were made, so we would have to make a backup copy
 * for stateT, which might be expensive. The real show stopper is that there
 * is no way to guarantee that from and from_end are valid pointers without
 * allocating memory, as in new[from_end-from]. This could be problematic
 * if from_end-from is on the order of gigabytes.
 *
 * So, to use a codecvt-type facet, we would have to inherit and add some
 * extra member functions, so what we would end up with is not a codecvt
 * facet.
 *
 * As a consequence of these considerations, we will develop our own
 * interface for the coder parameter classes.
 */

/*
 * CoderT is a class with the following methods:
 *
 * 	CoderT()
 * 	~CoderT()
 * void in(char c)
 * bool available()
 * char out()
 * bool at_end()
 * void end()
 *
 * The semantics of these methods are as follows:
 *
 * 		CoderT()
 * 		while(){
 * 			while(available()) out();
 *			if (at_end()) break;
 * 			in(c);
 * 		}
 *
 * The CoderT class represents some transformation to a character stream.
 * There are 2 types of streams to consider:
 *
 * 		self-terminating streams - these are streams with some in-band marker
 * 																which indicates completion of the stream. For these
 * 																streams, once they have entered their termination state,
 * 																a call to end() will have no effect. Calling end()
 * 																before they enter their termination may/may not be an error.
 * 		non-self-terminating streams - these streams require some out-of-band marker
 * 																		for stream termination. This marker is supplied
 * 																		by calling end(), which places the stream in a
 * 																		termination state. Any further character input
 * 																		after entering the termination state will
 * 																		throw.
 */

template<typename CharT, typename Traits>
  class basic_transitbuf_base : public std::basic_streambuf<CharT, Traits> {
public:
    typedef CharT                     	      char_type;
    typedef Traits                    	      traits_type;
    typedef std::basic_streambuf<char_type, traits_type> 	__streambuf_type;

    virtual size_t encoded_length(size_t n)=0;
    virtual size_t null_encoded_length(size_t n)=0;
    virtual std::basic_streambuf<CharT, Traits> *
    open( std::ios_base::openmode open_mode,__streambuf_type *sbuf, const char *prog=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL, const char *arg4=NULL)=0;
    virtual bool is_open(void)=0;

    virtual std::basic_streambuf<CharT, Traits> *
    close()=0;
    virtual std::basic_streambuf<char, std::char_traits<char> >*
    rdbuf(void)=0;

    virtual std::basic_streambuf<char, std::char_traits<char> >*
    rdbuf(std::basic_streambuf<char, std::char_traits<char> >* newbuf)=0;
};

  template<typename CharT, typename Traits, typename CodecT>
    class basic_transitbuf : public basic_transitbuf_base<CharT, Traits> {
      protected:
          CodecT codec;
      public:
          // Types:
          typedef CharT                             char_type;
          typedef Traits                            traits_type;
          typedef typename traits_type::int_type      int_type;
          typedef typename traits_type::pos_type      pos_type;
          typedef typename traits_type::off_type      off_type;

          typedef std::basic_streambuf<char_type, traits_type>    __streambuf_type;
          typedef typename traits_type::state_type                    __state_type;
          typedef std::codecvt<char_type, char, __state_type>     __codecvt_type;

          friend class ios_base; // For sync_with_stdio.

          typedef std::ios_base::openmode openmode;
      protected:
          std::basic_streambuf<char, std::char_traits<char> >*    next_streambuf;
          openmode mode;
          bool is_open_flag;

          static const size_t default_pback_size=1;
          static const size_t default_buffer_size=1024;

          size_t pback_size;
          size_t buffer_size;

          char *buffer;
          boost::scoped_array<char> buffer_manager;
      public:
          basic_transitbuf(void)
            : next_streambuf(0),
              mode(static_cast<std::ios_base::openmode>(0)),
              is_open_flag(false),
              pback_size(default_pback_size),
              buffer_size(default_buffer_size),
              buffer(0),
              buffer_manager(0),
              codec() { }

          template<typename CodecInitializer>
          basic_transitbuf(CodecInitializer i)
                  : pback_size(default_pback_size), buffer_size(default_buffer_size),
                      next_streambuf(0), codec(i) { }

          ~basic_transitbuf(){
              if (is_open()) close();
          }

          std::basic_streambuf<CharT, Traits>*
          open( std::ios_base::openmode open_mode,
                  __streambuf_type *sbuf, const char *prog, const char *arg1, const char *arg2, const char *arg3, const char *arg4){
              mode=open_mode;
              next_streambuf=sbuf;
              is_open_flag=true;

              buffer_manager.reset(new char[pback_size+buffer_size]);
              buffer=buffer_manager.get();

              setg(   buffer+pback_size+buffer_size,
                      buffer+pback_size+buffer_size,
                      buffer+pback_size+buffer_size);
              setp(   buffer+pback_size+buffer_size,
                      buffer+pback_size+buffer_size);

              codec.open();

              return this;
          }

          bool is_open(void){
              return is_open_flag;
          }

          std::basic_streambuf<CharT, Traits>*
          close(){
              if (mode==std::ios_base::out){
                  overflow();
                  codec.end();
                  overflow();
              }

              is_open_flag=false;

              buffer_manager.reset();

              return this;
          }

          std::basic_streambuf<char, std::char_traits<char> >* rdbuf(void){
              return next_streambuf;
          }

          std::basic_streambuf<char, std::char_traits<char> >* rdbuf(std::basic_streambuf<char, std::char_traits<char> >* newbuf){
              std::basic_streambuf<char, std::char_traits<char> >* tmp=next_streambuf;
              next_streambuf=newbuf;

              return tmp;
          }
              /*
               * setbuf(0,0)
               *
               * setbuf(p!=0,n)
               */
          std::basic_streambuf<CharT, Traits> *
          setbuf(char_type *p, off_type n){
              if (p==0 && n==0){
                  throw "setbuf(0,0) not implemented";
              }
              else if (p==0 && n!=0){
                  buffer_manager.reset(new char[n]);
                  buffer=buffer_manager.get();

                  if (mode & std::ios_base::out){
                      buffer_size=n;

                      std::basic_streambuf<CharT, Traits>::setp(  buffer+buffer_size,
                                          buffer+buffer_size);
                  }
                  else if (mode & std::ios_base::in){
                      buffer_size=n-pback_size;

                      setg(   buffer+pback_size+buffer_size,
                                  buffer+pback_size+buffer_size,
                                  buffer+pback_size+buffer_size);
                  }
              }
              else if (p!=0 && n!=0){
                  throw "setbuf(p!=0,n!=0) not implemented";
              }
          }

              /*
               * move the last characters in the input buffer to the putback area
               * before call to shift_gbuffer:
               *        A  B  C  D  E  F  G  H  I  J  K  L  M  N  O
               *              ^                                           ^
               *          eback                                        epptr
               *                                                   gptr
               *
               * after call to shift_gbuffer:
               *        M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  a
               *              ^        ^                                  ^
               *          eback     gptr                               epptr
               *
               */

          void shift_gbuffer(void){
              ptrdiff_t ptrdiff=egptr()-eback();
              if (ptrdiff<0)
                  throw "basic_transitbuf::shift_gbuffer: egptr()<=eback()";
              int n_putback=std::min(pback_size,static_cast<size_t>(ptrdiff));

              for(int i=0; i<n_putback; ++i)
                  buffer[pback_size+i-n_putback]=egptr()[i-n_putback];

              setg(buffer-n_putback+pback_size, buffer+pback_size, buffer+pback_size);
          }

          /*
           * move any characters not yet output to the beginning of the put buffer
               * before call to shift_gbuffer:
               *        A  B  C  D  E  F  G  H  _  _  _  _  _  _  _
               *                          ^           ^                   ^
               *                      pbase        pptr                epptr
               *
               * after call to shift_gbuffer:
               *        E  F  G  H  _  _  _  _  _  _  _  _  _  _  _
               *              ^           ^                               ^
               *          pbase        pptr                            epptr
               */

          void shift_pbuffer(void){
              ptrdiff_t n=pptr()-pbase();

              if (n<0)
                  throw "basic_transitbuf::shift_pbuffer: pptr()<pbase()";
              for(size_t i=0; i<n; ++i)
                  buffer[i]=pbase()[i];

              setp(buffer,buffer+n,buffer+buffer_size);
          }

          using std::basic_streambuf<CharT, Traits>::setg;
          using std::basic_streambuf<CharT, Traits>::setp;

          void setp(char_type *pb, char_type *pp, char_type *ep){
              setp(pb, ep);
              pbump(pp-pb);
          }

          using std::basic_streambuf<CharT, Traits>::pbase;
          using std::basic_streambuf<CharT, Traits>::pptr;
          using std::basic_streambuf<CharT, Traits>::epptr;

          using std::basic_streambuf<CharT, Traits>::eback;
          using std::basic_streambuf<CharT, Traits>::gptr;
          using std::basic_streambuf<CharT, Traits>::egptr;

          using std::basic_streambuf<CharT, Traits>::gbump;
          using std::basic_streambuf<CharT, Traits>::pbump;

          int_type sync(void){
              overflow();

              return 0;
          }

          int_type overflow(int_type c = traits_type::eof()){
              for( ; pbase()<pptr() ; ){
                  while(codec.available())
                      if (traits_type::eq_int_type(next_streambuf->sputc(codec.out()),traits_type::eof()))
                          return traits_type::eof();
                  if (codec.at_end())
                      if (pbase()<pptr()) throw ;                     // coder has terminated, but characters remain in the output buffer
                      else break;
                  else codec.in(*pbase());
                  setp(pbase()+1,pptr(),epptr());
              }

              while(codec.available()){
                  int_type tmp=next_streambuf->sputc(codec.out());
                  if (traits_type::eq_int_type(tmp,traits_type::eof()))
                      return traits_type::eof();
              }

              setp(buffer,buffer+pback_size+buffer_size);
              if (!traits_type::eq_int_type(c,traits_type::eof()))
                if (codec.at_end())
                  throw ;
                else if (pbase()<epptr()){
                      *pptr()=traits_type::to_char_type(c);
                      pbump(1);
                  }
                  else
                      codec.in(traits_type::to_char_type(c));

              return traits_type::not_eof(c);
          }

          int_type underflow(){
              shift_gbuffer();

              for( ; ; ){
                  if (egptr()>=buffer+buffer_size+pback_size) break;

                  while(codec.available()){
                      if (egptr()>=buffer+buffer_size+pback_size) break;
                      else{
                          *egptr()=codec.out();
                          setg(eback(),gptr(),egptr()+1);
                      }
                  }

                  if (egptr()>=buffer+buffer_size+pback_size) break;

                      if (codec.at_end()) break;

                      int_type c_int=next_streambuf->sgetc();

                      if (traits_type::eq_int_type(   c_int,traits_type::eof())){
                          codec.end();
                          break;
                      }
                      next_streambuf->sbumpc();

                      char c=traits_type::to_char_type(c_int);
                      codec.in(c);
              }

              if (gptr()<egptr()) return traits_type::to_int_type(*gptr());
              else return traits_type::eof();
          }

          int_type pbackfail(int_type c){
              if (gptr()<=eback())
                  return traits_type::eof();                                                      // attempting to push back beyond eback()

              setg(eback(), gptr()-1, egptr());

              if (!traits_type::eq_int_type(c,traits_type::eof()))
                  *gptr()=traits_type::to_char_type(c);

              return traits_type::not_eof(c);
          }

          size_t delta_encoded_length(size_t delta){ return codec.delta_encoded_length(delta); }
          size_t encoded_length(size_t delta){ return codec.encoded_length(delta); }
          size_t null_encoded_length(size_t delta){ return codec.null_encoded_length(delta); }
    };

  typedef basic_transitbuf_base<char,std::char_traits<char> > transitbuf_base;

#endif /* basic_transitbuf_hpp_ */
