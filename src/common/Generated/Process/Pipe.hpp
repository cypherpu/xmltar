/*
 * Pipe.hpp
 *
 *  Created on: Feb 9, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_PROCESS_PIPE_HPP_
#define SRC_COMMON_PROCESS_PIPE_HPP_

#include <unistd.h>			// ::pipe, ::execve
#include <string.h>			// strerror
#include <fcntl.h>			// fcntl

#include <array>

class Pipe {
	int readfd_;
	int writefd_;

	size_t readCount_;
	size_t writeCount_;
public:
	Pipe()
		: readfd_(-1), writefd_(-1), readCount_(0), writeCount_(0){}

	int pipe(){
		int fds[2];
		int result=::pipe(fds);

		readfd_=fds[0];
		writefd_=fds[1];

		return result;
	}

	int readfd(){
		return readfd_;
	}

	int writefd(){
		return writefd_;
	}

	void closeRead(){
		if (close(readfd_))
			throw std::runtime_error(std::string("Pipe::closeRead: ")+strerror(errno));
	}

	void closeWrite(){
		if (close(writefd_))
			throw std::runtime_error(std::string("Pipe::closeWrite: ")+strerror(errno));
	}

	void stdin(){
		if (close(0))
			throw std::runtime_error(std::string("Pipe::stdin: cannot close stdin: ")+strerror(errno));
		if (dup(readfd_)==-1)
			throw std::runtime_error(std::string("Pipe::stdin: cannot dup: ")+strerror(errno));
		if (close(readfd_))
			throw std::runtime_error(std::string("Pipe::stdin: cannot close pipe read end: ")+strerror(errno));
	}

	void stdout(){
		if (close(1))
			throw std::runtime_error(std::string("Pipe::stdout: cannot close stdout: ")+strerror(errno));
		if (dup(writefd_)==-1)
			throw std::runtime_error(std::string("Pipe::stdout: cannot dup: ")+strerror(errno));
		if (close(writefd_))
			throw std::runtime_error(std::string("Pipe::stdout: cannot close pipe write end: ")+strerror(errno));
	}

	void stderr(){
		if (close(2))
			throw std::runtime_error(std::string("Pipe::stderr: cannot close stderr: ")+strerror(errno));
		if (dup(writefd_)==-1)
			throw std::runtime_error(std::string("Pipe::stderr: cannot dup: ")+strerror(errno));
		if (close(writefd_))
			throw std::runtime_error(std::string("Pipe::stderr: cannot close pipe write end: ")+strerror(errno));
	}

	void nonblockingWrite(){
		long int oldFlags;

        if ((oldFlags=::fcntl(writefd_,F_GETFL))<0)
            throw std::runtime_error(std::string("Pipe::nonblockingWrite: unable to ::fcntl(pipefd_.data()[1],F_GETFL): ")+strerror(errno));
        if (::fcntl(writefd_,F_SETFL,oldFlags | O_NONBLOCK)<0)
            throw std::runtime_error(std::string("Pipe::nonblockingWrite: unable to ::fcntl(pipefd_.data()[1],F_SETFL,old_flags | O_NONBLOCK)")+strerror(errno));
	}

	void nonblockingRead(){
		long int oldFlags;

        if ((oldFlags=::fcntl(readfd_,F_GETFL))<0)
            throw std::runtime_error("Pipe::nonblockingRead: unable to ::fcntl(pipefd_.data()[0],F_GETFL)");
        if (::fcntl(readfd_,F_SETFL,oldFlags | O_NONBLOCK)<0)
            throw std::runtime_error("Pipe::nonblockingRead: unable to ::fcntl(pipefd_.data()[0],F_SETFL,old_flags | O_NONBLOCK)");
	}

	size_t read(char *buffer, size_t n){
	    //if (read1DescriptorState_==DescriptorState::CLOSED) return 0;

	    ssize_t result=::read(readfd_,buffer,n);
	    //read1DescriptorState_=DescriptorState::OPENED;

	    if (result<0){
			if (errno==EAGAIN){
				// read is non-blocking but pipe is empty
				return 0;
			}
			else
				throw std::runtime_error(std::string("Pipe::read: ")+strerror(errno));
	    }

	    readCount_+=result;

	    //if (result==0 && getChildState()==ChildState::EXITED) close_read1();

	    return result;
	}

	std::string read(){
		char data[1024];

		ssize_t result=read(data,sizeof(data));
		return std::string(data,result);
	}

	size_t write(char const *data, size_t n){
		ssize_t result=::write(writefd_,data,n);
		// writeDescriptorState_=DescriptorState::OPENED;

		if (result<0)
			if (errno==EAGAIN){
				// write is non-blocking but pipe is full
				return 0;
			}
			else
				throw std::runtime_error(std::string("Pipe::write: ")+strerror(errno));
		else {
			writeCount_+=result;

			return result;
		}
	}

	std::string write(std::string const & s){
		ssize_t result=write(s.c_str(),s.size());

		return s.substr(result);
	}
};

#endif /* SRC_COMMON_PROCESS_PIPE_HPP_ */
