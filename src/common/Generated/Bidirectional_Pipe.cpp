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

#include "Debug.hpp"

void Bidirectional_Pipe::Init(const char *prog, const std::vector<const char *> argv){
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

        if (execv(prog,const_cast<char * const *>(&(argv[1])))<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to execv";
    }
    else if (child_pid_>0){
        long int old_flags;
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

        if (close(tmp1[1])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp1[1])";
        if ((old_flags=::fcntl(tmp1[0],F_GETFD))<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp1[0],F_GETFD)";
        if (::fcntl(tmp1[0],F_SETFD,old_flags | FD_CLOEXEC)<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp1[0],F_SETFD,old_flags | FD_CLOEXEC)";
        child_stdout_to_parent_=tmp1[0];

        if (close(tmp2[1])<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to close(tmp2[1])";
        if ((old_flags=::fcntl(tmp2[0],F_GETFD))<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp2[0],F_GETFD)";
        if (::fcntl(tmp2[0],F_SETFD,old_flags | FD_CLOEXEC)<0)
            throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to ::fcntl(tmp2[0],F_SETFD,old_flags | FD_CLOEXEC)";
        child_stderr_to_parent_=tmp2[0];

        std::cerr << DEBUGCXXTAB(debugcxx);
        Print_Args();
        std::cerr << std::endl;

    }
    else if (child_pid_==-1){
        throw "Bidirectional_Pipe::Bidirectional_Pipe: unable to fork";
    }

    exit_status_=0;
    child_alive_=true;
    pipe_state_=RUNNING;
    read1_count=0;
    write_count=0;
}

void Bidirectional_Pipe::Set_Child_Status(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Set_Child_Status()");

    if (pipe_state_==EXITED) return;

    int status;
    int waitpid_result=waitpid(child_pid_,&status,WNOHANG);

    if (waitpid_result<0)
        throw "Bidirectional_Pipe::Set_Child_Status: unable to waitpid";
    else if (waitpid_result==0) return;
    else if (waitpid_result==child_pid_){
        pipe_state_=EXITED;
        if (!WIFEXITED(status))
            throw "Bidirectional_Pipe::Set_Child_Status: abnormal exit from child";
        exit_status_=WEXITSTATUS(status);
    }
    else throw "Bidirection_Pipe::Set_Child_Status: unknown waitpid_result";
}

void Bidirectional_Pipe::Open(const char *prog, const char *argv1, const char *argv2, const char *argv3, const char *argv4){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Open");

    std::vector<const char *> argv;
    argv.push_back(prog);
    argv.push_back(argv1);
    argv.push_back(argv2);
    argv.push_back(argv3);
    argv.push_back(argv4);
    argv.push_back(0);

    Init(prog,argv);

    pipe_state_=RUNNING;
}

void Bidirectional_Pipe::Select_Helper(struct timeval *pt){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Select_Helper()");

    fd_set readfds;
    fd_set writefds;

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    if (parent_to_child_stdin_!=-1) FD_SET(parent_to_child_stdin_,&writefds);
    if (child_stdout_to_parent_!=-1) FD_SET(child_stdout_to_parent_,&readfds);
    if (child_stderr_to_parent_!=-1) FD_SET(child_stderr_to_parent_,&readfds);

    int nfds=1+std::max(parent_to_child_stdin_,
                        std::max(child_stdout_to_parent_,child_stderr_to_parent_));

    for( ; ; )
        if (select(nfds,&readfds,&writefds,NULL,pt)<0)
            if (errno==EINTR){
                std::cerr << DEBUGCXXTAB(debugcxx) << "Bidirectional_Pipe::get_state: select: EINTR" << std::endl;
                continue;
            }
            else {
                throw "Bidirectional_Pipe::get_state: cannot select";
            }
        else break;

    if (child_stdout_to_parent_!=-1 && FD_ISSET(child_stdout_to_parent_,&readfds)) can_read1=true;
    else can_read1=false;

    if (child_stderr_to_parent_!=-1 && FD_ISSET(child_stderr_to_parent_,&readfds)) can_read2=true;
    else can_read2=false;

    if (parent_to_child_stdin_!=-1 && FD_ISSET(parent_to_child_stdin_,&writefds)) can_write=true;
    else can_write=false;
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
    : parent_to_child_stdin_(-1), child_stdout_to_parent_(-1), child_stderr_to_parent_(-1),
      pipe_state_(UNOPENED), read1_count(0), write_count(0),
      can_read1(false), can_read2(false), can_write(false) {
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Bidirectional_Pipe()");

}

Bidirectional_Pipe::~Bidirectional_Pipe(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::~Bidirectional_Pipe()");

    if (pipe_state_==UNOPENED) return;

    if (parent_to_child_stdin_!=-1) close(parent_to_child_stdin_);
    if (child_stdout_to_parent_!=-1) close(child_stdout_to_parent_);
    if (child_stderr_to_parent_!=-1) close(child_stderr_to_parent_);
}

Bidirectional_Pipe::State Bidirectional_Pipe::get_state(void){
    return pipe_state_;
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

    if (parent_to_child_stdin_==-1) return;

    if (close(parent_to_child_stdin_)<0)
        throw "Bidirectional_Pipe::close_write: could not close parent_to_child_stdin_";

    parent_to_child_stdin_=-1;
}

void Bidirectional_Pipe::close_read1(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::close_read1");
    if (close(child_stdout_to_parent_)<0)
        throw "Bidirectional_Pipe::close_write: could not close child_to_parent_stdout_";
    child_stdout_to_parent_=-1;
}

void Bidirectional_Pipe::close_read2(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::close_read2");
    if (close(child_stderr_to_parent_)<0)
        throw "Bidirectional_Pipe::close_write: could not close child_to_parent_stderr_";
    child_stderr_to_parent_=-1;
}

void Bidirectional_Pipe::Print_Args(void){
    std::cerr << "[" << child_pid_ << "]:";
    for(int i=0; i<saved_args.size(); ++i)
        if (saved_args[i]!=NULL)
            std::cerr << saved_args[i] << " ";
}

bool Bidirectional_Pipe::Can_Read1(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Can_Read1()");

    return can_read1;
}

bool Bidirectional_Pipe::Can_Read2(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Can_Read2()");

    return can_read2;
}

bool Bidirectional_Pipe::Can_Write(void){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Can_Write()");

    return can_write;
}

ssize_t Bidirectional_Pipe::Write(const char *p, size_t n){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Write()");

    ssize_t result=::write(parent_to_child_stdin_,p,n);

    if (result<0)
    	throw "Bidirectional_Pipe::Write: write error";

    write_count+=result;

    return result;
}

std::string Bidirectional_Pipe::Read1(size_t n){
    DEBUGCXX(debugcxx,"Bidirectional_Pipe::Read1()");

    char buf[PIPE_BUF];

    ssize_t result=::read(child_stdout_to_parent_,buf,std::min(n,(size_t)PIPE_BUF));

    if (result<0)
    	throw "Bidirectional_Pipe::Read1: read error";

    read1_count+=result;

    return std::string(buf,result);
}
