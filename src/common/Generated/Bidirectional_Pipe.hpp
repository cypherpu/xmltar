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

#ifndef Bidirectional_Pipe_hpp_
#define Bidirectional_Pipe_hpp_

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <sys/wait.h>

#include <string>
#include <vector>
#include <deque>

#include "Utilities/ManagedBuffer.hpp"
#include "Debug/Debug.hpp"

class Bidirectional_Pipe {
public:
    enum class ChildState { NOT_STARTED, RUNNING, EXITED };
    enum class DescriptorState { NOT_OPENED, OPENED, OPENED_READABLE, OPENED_WRITABLE, CLOSED };
    static size_t pipe_buf_size;

public: // protected:
    int child_pid_;
    int exit_status_;
    ChildState childState_;

    int parent_to_child_stdin_;           // parent -> child::std::in
    int child_stdout_to_parent_;          // child::std::out -> parent
    int child_stderr_to_parent_;          // child::std::err -> parent

    size_t pipeRead1Count_;
    size_t pipeRead2Count_;
    size_t pipeWriteCount_;

    std::string saved_path_;
    std::vector<std::string> saved_args_;
    std::string name_;

    DescriptorState read1DescriptorState_;
    DescriptorState read2DescriptorState_;
    DescriptorState writeDescriptorState_;

    ManagedBuffer *write_;
    ManagedBuffer *read1_;
    ManagedBuffer *read2_;

    void Init(char const *path, std::vector<const char *> const & argv, std::string const & name, ManagedBuffer *write, ManagedBuffer *read1, ManagedBuffer *read2);
    void Set_Child_Status(void);
    void Select_Helper(struct timeval *pt);
    void Select_Nonblocking(void);
    void Select_Blocking(void);
    void Select_Blocking(unsigned int microseconds);

public:
    Bidirectional_Pipe(void);
    Bidirectional_Pipe(char const *path, std::vector<char const *> const & argv, std::string const & name="", ManagedBuffer *write=nullptr, ManagedBuffer *read1=nullptr, ManagedBuffer *read2=nullptr);

    ~Bidirectional_Pipe(void);

    void Open(const char *path, std::vector<char const *> argv, std::string const & name="", ManagedBuffer *write=nullptr, ManagedBuffer *read1=nullptr, ManagedBuffer *read2=nullptr);

    int SetFDs(fd_set & readfds, fd_set & writefds);
    void ReadFDs(fd_set & readfds, fd_set & writefds);
    ChildState getChildState(void);
    void close_write(void);
    void close_read1(void);
    void close_read2(void);

    size_t pipeRead1Count(void){ return pipeRead1Count_; }
    size_t pipeRead2Count(void){ return pipeRead2Count_; }
    size_t pipeWriteCount(void){ return pipeWriteCount_; }

    void Print_Args(void);

    bool Can_Read1(void);
    bool Can_Read2(void);
    bool Can_Write(void);

    size_t write(char const *data, size_t n);
    size_t read1(char *buffer, size_t n);
   	size_t read2(char *buffer, size_t n);

   	bool writeIsOpen(){
   		return writeDescriptorState_==DescriptorState::OPENED || writeDescriptorState_==DescriptorState::OPENED_WRITABLE;
   	}
   	bool read1IsOpen(){
   		return read1DescriptorState_==DescriptorState::OPENED || read1DescriptorState_==DescriptorState::OPENED_READABLE;
   	}
   	bool read2IsOpen(){
   		return read2DescriptorState_==DescriptorState::OPENED || read2DescriptorState_==DescriptorState::OPENED_READABLE;
   	}

    bool ChildExitedAndAllPipesClosed();
    int ExitStatus();

    explicit operator bool();

    size_t pipeBufSize(int fd){
    	return fcntl(fd,F_GETPIPE_SZ);
    }

    friend std::ostream & operator<<(std::ostream & os, ChildState p);
    friend std::ostream & operator<<(std::ostream & os, DescriptorState p);
};

#endif /* Bidirectional_Pipe_hpp_ */
