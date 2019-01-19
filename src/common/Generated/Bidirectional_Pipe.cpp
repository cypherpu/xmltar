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

#include "Bidirectional_Pipe.hpp"

#include <iostream>
#include <errno.h>
#include <string.h>

#include "Debug/Debug.hpp"

void Bidirectional_Pipe::Init(const char *path, const std::vector<const char *> argv){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Init");
    int tmp0[2], tmp1[2], tmp2[2];

    saved_args=argv;

    if (::pipe(tmp0)<0)
        throw "Bidirectional_Pipe::Bidirectional_Pipe: cannot create tmp0";
    if (::pipe(tmp1)<0)
        throw "Bidirectional_Pipe::Bidirectional_Pipe: cannot create tmp1";
    if (::pipe(tmp2)<0)
        throw "Bidirectional_Pipe::Bidirectional_Pipe: cannot create tmp2";

    child_pid_=fork();
    if (child_pid_==0){
        if (close(tmp0[1])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp0[1])";
        if (close(0)<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(0)";
        if (dup(tmp0[0])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to dup(tmp0[1])";
        if (close(tmp0[0])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp0[1])";

        if (close(tmp1[0])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp1[0])";
        if (close(1)<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(1)";
        if (dup(tmp1[1])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to dup(tmp1[0])";
        if (close(tmp1[1])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp1[0])";

        if (close(tmp2[0])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp2[0])";
        if (close(2)<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(2)";
        if (dup(tmp2[1])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to dup(tmp2[1])";
        if (close(tmp2[1])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp2[1])";

        if (execv(path,const_cast<char * const *>(&(argv[0])))<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to execv";
    }
    else if (child_pid_>0){
        long int old_flags;

        // In the parent,
        //		we don't want to read the child:stdin
        if (close(tmp0[0])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp0[0])";
#if 1
        if ((old_flags=::fcntl(tmp0[1],F_GETFL))<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp0[1],F_GETFL)";
        if (::fcntl(tmp0[1],F_SETFL,old_flags | O_NONBLOCK)<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp0[1],F_SETFL,old_flags | O_NONBLOCK)";
#endif
        if ((old_flags=::fcntl(tmp0[1],F_GETFD))<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp0[1],F_GETFD)";
        if (::fcntl(tmp0[1],F_SETFD,old_flags | FD_CLOEXEC)<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp0[1],F_SETFD,old_flags | FD_CLOEXEC)";
        parent_to_child_stdin_=tmp0[1];

        //		we don't want to write the child:stdout
        if (close(tmp1[1])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp1[1])";
        if ((old_flags=::fcntl(tmp1[0],F_GETFD))<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp1[0],F_GETFD)";
        if (::fcntl(tmp1[0],F_SETFD,old_flags | FD_CLOEXEC)<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp1[0],F_SETFD,old_flags | FD_CLOEXEC)";
        child_stdout_to_parent_=tmp1[0];

        //		we don't want to write the child:stderr
        if (close(tmp2[1])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp2[1])";
        if ((old_flags=::fcntl(tmp2[0],F_GETFD))<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp2[0],F_GETFD)";
        if (::fcntl(tmp2[0],F_SETFD,old_flags | FD_CLOEXEC)<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp2[0],F_SETFD,old_flags | FD_CLOEXEC)";
        child_stderr_to_parent_=tmp2[0];

        std::cerr << DEBUGCXXTAB(debugcxx);
        //Print_Args();
        //std::cerr << std::endl;

        // std::cerr << "fd " << parent_to_child_stdin_ << " " << child_stdout_to_parent_ << " " << child_stderr_to_parent_ << std::endl;

    }
    else if (child_pid_==-1){
        throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to fork";
    }

    exit_status_=0;

    childState_=ChildState::RUNNING;

    pipeRead1Count_=0;
    pipeRead2Count_=0;
    pipeWriteCount_=0;

    read1DescriptorState_=DescriptorState::OPENED;
    read2DescriptorState_=DescriptorState::OPENED;
    writeDescriptorState_=DescriptorState::OPENED;
}

void Bidirectional_Pipe::Set_Child_Status(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Set_Child_Status()");

    if (childState_==ChildState::EXITED) return;

    int status;
    int waitpid_result=waitpid(child_pid_,&status,WNOHANG);

    if (waitpid_result<0){
    	std::cerr << strerror(errno) << std::endl;
        throw "Bidirectional_Pipe::Set_Child_Status: unable to waitpid";
    }
    else if (waitpid_result==0) return;
    else if (waitpid_result==child_pid_){
    	//std::cerr << " EXITED ";
    	childState_=ChildState::EXITED;
        if (!WIFEXITED(status))
            throw "Bidirectional_Pipe::Set_Child_Status: abnormal exit from child";
        exit_status_=WEXITSTATUS(status);
    }
    else throw "Bidirection_Pipe::Set_Child_Status: unknown waitpid_result";
}

void Bidirectional_Pipe::Open(char const *path, std::vector<char const *> argv){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Open");

    if (childState_==ChildState::RUNNING)
    	throw "Bidirectional_Pipe::Open: child already opened";

    argv.push_back(nullptr);
    Init(path,argv);

    childState_=ChildState::RUNNING;
}

void Bidirectional_Pipe::Select_Helper(struct timeval *pt){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Select_Helper()");

    fd_set readfds;
    fd_set writefds;

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    if (writeDescriptorState_==DescriptorState::OPENED || writeDescriptorState_==DescriptorState::OPENED_WRITABLE) FD_SET(parent_to_child_stdin_,&writefds);
    if (read1DescriptorState_==DescriptorState::OPENED || read1DescriptorState_==DescriptorState::OPENED_READABLE) FD_SET(child_stdout_to_parent_,&readfds);
    if (read2DescriptorState_==DescriptorState::OPENED || read2DescriptorState_==DescriptorState::OPENED_READABLE) FD_SET(child_stderr_to_parent_,&readfds);

    int nfds=1+std::max(parent_to_child_stdin_,
                        std::max(child_stdout_to_parent_,child_stderr_to_parent_));

    for( ; ; )
        if (select(nfds,&readfds,&writefds,NULL,pt)<0)
            if (errno==EINTR){
                // std::cerr << DEBUGCXXTAB(debugcxx) << "Bidirectional_Pipe::get_state: select: EINTR" << std::endl;
                continue;
            }
            else {
            	std::cerr << "error number=" << errno << " " << strerror(errno) << std::endl;
                throw "Bidirectional_Pipe::get_state: cannot select";
            }
        else break;

    if (writeDescriptorState_==DescriptorState::OPENED || writeDescriptorState_==DescriptorState::OPENED_WRITABLE){
    	if (FD_ISSET(parent_to_child_stdin_,&writefds))
			writeDescriptorState_=DescriptorState::OPENED_WRITABLE;
		else
			writeDescriptorState_=DescriptorState::OPENED;
    }

    if (read1DescriptorState_==DescriptorState::OPENED || read1DescriptorState_==DescriptorState::OPENED_READABLE){
    	if (FD_ISSET(child_stdout_to_parent_,&readfds))
			read1DescriptorState_=DescriptorState::OPENED_READABLE;
		else
			read1DescriptorState_=DescriptorState::OPENED;
    }

    if (read2DescriptorState_==DescriptorState::OPENED || read2DescriptorState_==DescriptorState::OPENED_READABLE){
    	if (FD_ISSET(child_stderr_to_parent_,&readfds))
			read2DescriptorState_=DescriptorState::OPENED_READABLE;
		else
			read2DescriptorState_=DescriptorState::OPENED;
    }

    // std::cerr << "\n" << read1DescriptorState_ << " " << read2DescriptorState_ << " " << writeDescriptorState_ << "  ";
}

void Bidirectional_Pipe::Select_Nonblocking(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Select_Nonblocking()");
    struct timeval t;
    t.tv_sec=0;
    t.tv_usec=0;

    Select_Helper(&t);
}

void Bidirectional_Pipe::Select_Blocking(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Select_Blocking()");
    Select_Helper(NULL);
}

void Bidirectional_Pipe::Select_Blocking(unsigned int microseconds){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Select_Blocking(int)");
    struct timeval t;
    t.tv_sec=0;
    t.tv_usec=microseconds;

    Select_Helper(&t);
}

Bidirectional_Pipe::Bidirectional_Pipe(void)
    : child_pid_(-1), exit_status_(-1), childState_(ChildState::NOT_STARTED),
	  parent_to_child_stdin_(-1), child_stdout_to_parent_(-1), child_stderr_to_parent_(-1),
	  pipeRead1Count_(0), pipeRead2Count_(0), pipeWriteCount_(0),
	  read1DescriptorState_(DescriptorState::NOT_OPENED), read2DescriptorState_(DescriptorState::NOT_OPENED), writeDescriptorState_(DescriptorState::NOT_OPENED){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Bidirectional_Pipe()");
}

Bidirectional_Pipe::Bidirectional_Pipe(const char *path, std::vector<char const *> argv)
    : Bidirectional_Pipe() {
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Bidirectional_Pipe()");
    Open(path,argv);

}

Bidirectional_Pipe::~Bidirectional_Pipe(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::~Bidirectional_Pipe()");

    if (childState_==ChildState::NOT_STARTED) return;

    // std::cerr << writeDescriptorState_ << " " << read1DescriptorState_ << " " << read2DescriptorState_ << std::endl;

    if (writeDescriptorState_==DescriptorState::OPENED || writeDescriptorState_==DescriptorState::OPENED_WRITABLE) close(parent_to_child_stdin_);
    if (read1DescriptorState_==DescriptorState::OPENED || read1DescriptorState_==DescriptorState::OPENED_READABLE) close(child_stdout_to_parent_);
    if (read2DescriptorState_==DescriptorState::OPENED || read2DescriptorState_==DescriptorState::OPENED_READABLE) close(child_stderr_to_parent_);
}

Bidirectional_Pipe::ChildState Bidirectional_Pipe::getChildState(void){
    return childState_;
}
/*
 * @brief close a file descriptor and mark as unusable
 *
 * To close the child's stdin pipe, we would like semantics that
 * ensure that all bytes written from the parent have actually
 * entered the pipe. So, we require the write buffer to be empty,
 * or equivalently, for get_state() to return WRITABLE
 */

void Bidirectional_Pipe::close_write(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::close_write");

    if (writeDescriptorState_==DescriptorState::NOT_OPENED || writeDescriptorState_==DescriptorState::CLOSED) return;

    if (close(parent_to_child_stdin_)<0)
        throw "Bidirectional_Pipe::close_write: could not close parent_to_child_stdin_";

    writeDescriptorState_=DescriptorState::CLOSED;
}

void Bidirectional_Pipe::close_read1(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::close_read1");
    if (close(child_stdout_to_parent_)<0)
        throw "Bidirectional_Pipe::close_read1: could not close child_to_parent_stdout_";

    read1DescriptorState_=DescriptorState::CLOSED;
}

void Bidirectional_Pipe::close_read2(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::close_read2");
    if (close(child_stderr_to_parent_)<0)
        throw "Bidirectional_Pipe::close_read2: could not close child_to_parent_stderr_";

    read2DescriptorState_=DescriptorState::CLOSED;
}

void Bidirectional_Pipe::Print_Args(void){
    std::cerr << "[" << child_pid_ << "]:";
    for(size_t i=0; i<saved_args.size(); ++i)
        if (saved_args[i]!=NULL)
            std::cerr << saved_args[i] << " ";
}

bool Bidirectional_Pipe::Can_Read1(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Can_Read1()");

    return read1DescriptorState_==DescriptorState::OPENED_READABLE;
}

bool Bidirectional_Pipe::Can_Read2(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Can_Read2()");

    return read2DescriptorState_==DescriptorState::OPENED_READABLE;
}

bool Bidirectional_Pipe::Can_Write(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Can_Write()");

    return writeDescriptorState_==DescriptorState::OPENED_WRITABLE;
}

size_t Bidirectional_Pipe::write(char const *data, size_t n){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::write(char const *data, size_t n)");

	ssize_t result=::write(parent_to_child_stdin_,data,n);
	writeDescriptorState_==DescriptorState::OPENED;

	if (result<0)
		if (errno==EAGAIN){
			// write is non-blocking but pipe is full
			return 0;
		}
		else {
			std::cerr << strerror(errno) << std::endl;
			throw "Bidirectional_Pipe::Write: write error";
		}
	else {
		pipeWriteCount_+=result;

		return result;
	}
}

size_t Bidirectional_Pipe::read1(char *buffer, size_t n){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::read1(char *buffer, size_t n)");

    if (read1DescriptorState_==DescriptorState::CLOSED) return 0;

    size_t result=::read(child_stdout_to_parent_,buffer,n);
    read1DescriptorState_==DescriptorState::OPENED;

    if (result<0)
    	throw "Bidirectional_Pipe::read1: read error";

    pipeRead1Count_+=result;

    if (result==0 && getChildState()==ChildState::EXITED) close_read1();

    return result;
}

size_t Bidirectional_Pipe::read2(char *buffer, size_t n){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::read2(char *buffer, size_t n)");

    if (read2DescriptorState_==DescriptorState::CLOSED) return 0;

    size_t result=::read(child_stderr_to_parent_,buffer,n);
    read2DescriptorState_==DescriptorState::OPENED;

    if (result<0)
    	throw "Bidirectional_Pipe::read2: read error";

    pipeRead2Count_+=result;

    if (result==0 && getChildState()==ChildState::EXITED) close_read2();

    return result;
}

bool Bidirectional_Pipe::ChildExitedAndAllPipesClosed(){
	Set_Child_Status();
	Select_Nonblocking();

	if (childState_==ChildState::EXITED &&
		read1DescriptorState_==DescriptorState::CLOSED &&
		read2DescriptorState_==DescriptorState::CLOSED &&
		writeDescriptorState_==DescriptorState::CLOSED)
		return true;

	return false;
}

int Bidirectional_Pipe::ExitStatus(){
	if (childState_==ChildState::EXITED)
		return exit_status_;
	else throw std::logic_error("BidirectionalPipe::ExitStatus: called before child has exited");
}

std::ostream & operator<<(std::ostream & os, Bidirectional_Pipe::ChildState c){
	switch(c){
	case Bidirectional_Pipe::ChildState::NOT_STARTED:
		os << "NOT_STARTED";
		break;
	case Bidirectional_Pipe::ChildState::RUNNING:
		os << "RUNNING    ";
		break;
	case Bidirectional_Pipe::ChildState::EXITED:
		os << "EXITED     ";
		break;
	default:
		throw "operator<<: invalid ChildState";
	}

	return os;
}

std::ostream & operator<<(std::ostream & os, Bidirectional_Pipe::DescriptorState p){
	switch(p){
	case Bidirectional_Pipe::DescriptorState::NOT_OPENED:
		os << "NOT_OPENED      ";
		break;
	case Bidirectional_Pipe::DescriptorState::OPENED:
		os << "OPENED          ";
		break;
	case Bidirectional_Pipe::DescriptorState::OPENED_READABLE:
		os << "OPENED_READABLE ";
		break;
	case Bidirectional_Pipe::DescriptorState::OPENED_WRITABLE:
		os << "OPENED_WRITABLE ";
		break;
	case Bidirectional_Pipe::DescriptorState::CLOSED:
		os << "CLOSED          ";
		break;
	default:
		throw "operator<<: invalid PipeState";
	}

	return os;
}
