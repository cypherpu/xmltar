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

#include "Debug/Debug.hpp"

class Bidirectional_Pipe {
public:
    enum class ChildState { NOT_STARTED, RUNNING, EXITED };
    enum class DescriptorState { NOT_OPENED, OPENED, OPENED_READABLE, OPENED_WRITABLE, CLOSED };

    friend std::ostream & operator<<(std::ostream & os, ChildState p);
    friend std::ostream & operator<<(std::ostream & os, DescriptorState p);

public: // protected:
    int child_pid_;
    int exit_status_;

    int parent_to_child_stdin_;           // parent -> child::std::in
    int child_stdout_to_parent_;          // child::std::out -> parent
    int child_stderr_to_parent_;          // child::std::err -> parent

    ChildState childState_;
    size_t read1_count;
    size_t read2_count;
    size_t write_count;

    size_t pipeRead1Count_;
    size_t pipeRead2Count_;

    size_t queued_write_count;

    static const size_t pipe_buf_size=PIPE_BUF;
    std::vector<char const *> saved_args;

    DescriptorState read1DescriptorState_;
    DescriptorState read2DescriptorState_;
    DescriptorState writeDescriptorState_;

    void Init(const char *path, const std::vector<const char *> argv);
    void Set_Child_Status(void);
    void Select_Helper(struct timeval *pt);
    void Select_Nonblocking(void);
    void Select_Blocking(void);
    void Select_Blocking(unsigned int microseconds);

    std::string writeBuffer_;
    bool writeCloseWhenEmpty_;
public:
    Bidirectional_Pipe(void);
    Bidirectional_Pipe(const char *path, std::vector<char const *> argv);

    ~Bidirectional_Pipe(void);

    void Open(const char *path, std::vector<char const *> argv);

    ChildState getChildState(void);
    void close_write(void);
    void close_read1(void);
    void close_read2(void);

    size_t Read1_Count(void){ return read1_count; }
    size_t Read2_Count(void){ return read2_count; }
    size_t pipeRead1Count(void){ return pipeRead1Count_; }
    size_t pipeRead2Count(void){ return pipeRead2Count_; }
    size_t Write_Count(void){ return write_count; }
    size_t Queued_Write_Count(void){ return queued_write_count; }

    void Print_Args(void);

    bool Can_Read1(void);
    bool Can_Read2(void);
    bool Can_Write(void);

    void Write();
    void QueueWrite(char const c);
    void QueueWrite(char const *c, int n);
    void QueueWrite(std::string const & data);
    std::string Read1(size_t n=PIPE_BUF);
    std::string Read2(size_t n=PIPE_BUF);

    void QueueWriteClose();
    bool ChildExitedAndAllPipesClosed();
    int ExitStatus();

    explicit operator bool();
};

#endif /* Bidirectional_Pipe_hpp_ */
