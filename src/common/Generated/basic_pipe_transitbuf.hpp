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

#ifndef basic_pipe_transitbuf_hpp_
#define basic_pipe_transitbuf_hpp_

#include "basic_transitbuf.hpp"
#include "Bidirectional_Pipe.hpp"
#include "Debug.hpp"

template<typename CharT, typename Traits>
class basic_pipe_transitbuf_base : public basic_transitbuf_base<CharT, Traits> {
protected:
    Bidirectional_Pipe pipe;
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
    static const size_t default_buffer_size=PIPE_BUF;

    size_t pback_size;
    size_t buffer_size;

    char *buffer;
    boost::scoped_array<char> buffer_manager;
public:
    basic_pipe_transitbuf_base(void)
      : next_streambuf(0),
        mode(static_cast<std::ios_base::openmode>(0)),
        is_open_flag(false),
        pback_size(default_pback_size),
        buffer_size(default_buffer_size),
        buffer(0),
        buffer_manager(0),
        pipe() { }

    ~basic_pipe_transitbuf_base(){
      if (is_open()) close();
    }

    std::basic_streambuf<CharT, Traits>*
    open( std::ios_base::openmode open_mode,
          __streambuf_type *sbuf, const char *prog=NULL, const char *arg1=NULL, const char *arg2=NULL, const char *arg3=NULL, const char *arg4=NULL){
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

        pipe.Open(prog, arg1, arg2, arg3, arg4);

        return this;
    }

    bool is_open(void){
        DEBUGCXX(debugcxx,"basic_pipe_transitbuf_base::is_open");
        return is_open_flag;
    }

    std::basic_streambuf<CharT, Traits>*
    close(){
        DEBUGCXX(debugcxx,"basic_pipe_transitbuf_base::close");

        if (mode==std::ios_base::out){
            overflow();
            pipe.close_write();
            for( ; ; ) {
                pipe.Set_Child_Status();
                pipe.Select_Blocking();

                if (pipe.Can_Read1()){
                    std::string s=pipe.Read1();

                    if (s.empty())
                        if (pipe.get_state()==Bidirectional_Pipe::EXITED) break;

                    if (next_streambuf->sputn(s.c_str(),s.size())!=s.size())
                        throw "basic_pipe_streambuf::close: could not sputn";
                    continue;
                }
                else
                    if (pipe.get_state()==Bidirectional_Pipe::EXITED) break;
            }
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
       * before call to shift_pbuffer:
       *        A  B  C  D  E  F  G  H  _  _  _  _  _  _  _
       *                          ^           ^                   ^
       *                      pbase        pptr                epptr
       *
       * after call to shift_pbuffer:
       *        E  F  G  H  _  _  _  _  _  _  _  _  _  _  _
       *              ^           ^                               ^
       *          pbase        pptr                            epptr
       */

    void shift_pbuffer(void){
        ptrdiff_t n=pptr()-pbase();

        if (n<0)
          throw "basic_transitbuf::shift_pbuffer: pptr()<pbase()";
        for(size_t i=0; i<n; ++i)
          buffer[i+pback_size]=pbase()[i];

        setp(buffer+pback_size,buffer+n,buffer+buffer_size);
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
        DEBUGCXX(debugcxx,"basic_pipe_transitbuf_base::overflow");

        ssize_t n_written=0;
        for(char_type *p=pbase(); p<pptr(); ){
            pipe.Select_Blocking();

            if (pipe.Can_Write()){
                p+=pipe.Write(p,pptr()-p);
            }
            if (pipe.Can_Read1()){
                std::string s=pipe.Read1();

                if (next_streambuf->sputn(s.c_str(),s.size())!=s.size())
                    throw "basic_pipe_streambuf::overflow: could not sputn";
                continue;
            }
        }

        setp(buffer+pback_size,buffer+pback_size+buffer_size);
        if (!traits_type::eq_int_type(c,traits_type::eof()))
            if (pipe.get_state()==Bidirectional_Pipe::EXITED)
                throw ;
            else if (pbase()<epptr()){
                *pptr()=traits_type::to_char_type(c);
                pbump(1);
            }
            else {
                for( ; ; ){
                    pipe.Select_Blocking();

                    if (pipe.Can_Write()){
                        char_type cc=traits_type::to_char_type(c);
                        if (pipe.Write(&cc,1)!=1)
                            throw "basic_pipe_streambuf::overflow: could not write 1 character";
                        else break;
                    }
                    else if (pipe.Can_Read1()){
                        std::string s=pipe.Read1();

                        if (next_streambuf->sputn(s.c_str(),s.size())!=s.size())
                            throw "basic_pipe_streambuf::overflow: could not sputn";
                        continue;
                    }
                }
            }

        return traits_type::not_eof(c);
    }

    int_type underflow(){
        DEBUGCXX(debugcxx,"basic_pipe_transitbuf_base::underflow");

        if (gptr()>=egptr()){
			shift_gbuffer();

			for( ; ; ){
				pipe.Set_Child_Status();
				if (egptr()>=buffer+buffer_size+pback_size) break;

				pipe.Select_Blocking();
				if (pipe.Can_Read1()){
					std::string s=pipe.Read1(buffer+buffer_size+pback_size-egptr());
					if (s.empty()) break;

					//std::cerr << "r=\"" << s << "\"" << std::endl;
					for(std::string::iterator i=s.begin(); i!=s.end(); ++i){
						*egptr()=*i;
						setg(eback(),gptr(),egptr()+1);
					}
				}

				if (egptr()>=buffer+buffer_size+pback_size) break;

				if (pipe.get_state()==Bidirectional_Pipe::EXITED) break;

				if (pipe.Can_Write()){
					int_type c_int=next_streambuf->sgetc();

					if (traits_type::eq_int_type(c_int,traits_type::eof())){
						pipe.close_write();
						break;;
					}
					next_streambuf->sbumpc();

					char c=traits_type::to_char_type(c_int);
					//std::cerr << "w=\"" << c << "\"" << std::endl;
					pipe.Write(&c,1);
				}
			}
        }

        if (gptr()<egptr()) return traits_type::to_int_type(*gptr());
        else return traits_type::eof();
    }
#if 0
    int_type underflow(){
        DEBUGCXX(debugcxx,"basic_pipe_transitbuf_base::underflow");
        shift_gbuffer();

        for( ; ; ){
          Bidirectional_Pipe::State state;
          if (egptr()>=buffer+buffer_size+pback_size) break;

          while((state=pipe.get_state())==Bidirectional_Pipe::READABLE_1){
              if (egptr()>=buffer+buffer_size+pback_size) break;
              else{
                  std::string s=pipe.get1(buffer+buffer_size+pback_size-egptr());
                  for(std::string::iterator i=s.begin(); i!=s.end(); ++i){
                      *egptr()=*i;
                      setg(eback(),gptr(),egptr()+1);
                  }
              }
          }

          if (egptr()>=buffer+buffer_size+pback_size) break;

          if (state==Bidirectional_Pipe::EXITED) break;

          for(size_t i=0; i<PIPE_BUF; ++i){
              int_type c_int=next_streambuf->sgetc();

              if (traits_type::eq_int_type(c_int,traits_type::eof())){
                  pipe.close_write();
                  break;;
              }
              next_streambuf->sbumpc();

              char c=traits_type::to_char_type(c_int);
              pipe.put(std::string(1,c));
          }
        }

        if (gptr()<egptr()) return traits_type::to_int_type(*gptr());
        else return traits_type::eof();
    }
#endif
    int_type pbackfail(int_type c){
        if (gptr()<=eback())
          return traits_type::eof();                                                      // attempting to push back beyond eback()

        setg(eback(), gptr()-1, egptr());

        if (!traits_type::eq_int_type(c,traits_type::eof()))
          *gptr()=traits_type::to_char_type(c);

        return traits_type::not_eof(c);
    }
};

typedef basic_pipe_transitbuf_base<char,std::char_traits<char> > pipe_transitbuf_base;

#endif /* basic_pipe_transitbuf_hpp_ */
