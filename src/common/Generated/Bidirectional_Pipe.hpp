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

#include "Debug.hpp"

class Bidirectional_Pipe {
public:
    enum State { UNOPENED, RUNNING, EXITED };
public: // protected:
    int child_pid_;
    int exit_status_;
    bool child_alive_;
    int parent_to_child_stdin_;           // parent -> child::std::in
    int child_stdout_to_parent_;          // child::std::out -> parent
    int child_stderr_to_parent_;          // child::std::err -> parent
    State pipe_state_;
    size_t read1_count;
    size_t write_count;
    static const size_t pipe_buf_size=PIPE_BUF;
    std::vector<const char *> saved_args;
    bool can_read1;
    bool can_read2;
    bool can_write;

    void Init(const char *prog, const std::vector<const char *> argv);
    void Set_Child_Status(void);
    void Select_Helper(struct timeval *pt);
    void Select_Nonblocking(void);
    void Select_Blocking(void);
    void Select_Blocking(unsigned int microseconds);
public:
    Bidirectional_Pipe(void);

    ~Bidirectional_Pipe(void);

    void Open(const char *prog, const char *argv1=NULL, const char *argv2=NULL, const char *argv3=NULL, const char *argv4=NULL);

    State get_state(void);
    void close_write(void);
    void close_read1(void);
    void close_read2(void);

    size_t Read1_Count(void){ return read1_count; }
    size_t Write_Count(void){ return write_count; }

    void Print_Args(void);

    bool Can_Read1(void);
    bool Can_Read2(void);
    bool Can_Write(void);

    ssize_t Write(const char *p, size_t n);
    std::string Read1(size_t n=PIPE_BUF);
};

#endif /* Bidirectional_Pipe_hpp_ */
