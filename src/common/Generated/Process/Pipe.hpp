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

#include <iostream> 		// printf debugging

#include <array>

class Descriptor {
public:
	enum State { UNOPENED, OPENED, CLOSED };

	State state_;
	int fd_;
	size_t count_;

	Descriptor()
		: state_(UNOPENED), fd_(-1), count_(0) {}

	void open(int fd){
		state_=OPENED;
		fd_=fd;
		count_=0;
		std::cerr << "Descriptor::open: fd_=" << fd_ << std::endl;
	}

	void close(){
		//std::cerr << "Descriptor: " << fd_ << " closed" << std::endl;
		state_=CLOSED;
		if (::close(fd_))
			throw std::runtime_error(std::string("Descriptor::close: ")+strerror(errno));
		std::cerr << "Descriptor::close: fd_=" << fd_ << std::endl;
	}

	void closeAndDup(int newFd){
		if (::close(newFd))
			throw std::runtime_error(std::string("Descriptor::close: ")+strerror(errno));
		if (dup(fd_)==-1)
			throw std::runtime_error(std::string("Descriptor::close: cannot dup: ")+strerror(errno));
		close();
		open(newFd);
	}

	friend std::ostream & operator<<(std::ostream & os, State state){
		if (state==UNOPENED) os << "UNOPENED";
		else if (state==OPENED) os << "OPENED";
		else if (state==CLOSED) os << "CLOSED";
		else throw std::runtime_error("operator<<(std::ostream & os, State state): unknown state");

		return os;
	}
};

class Pipe {
	Descriptor read_;
	Descriptor write_;
public:
	Pipe()
		: read_(), write_() {}

	int pipe(){
		int fds[2];
		int result=::pipe(fds);

		read_.open(fds[0]);
		write_.open(fds[1]);

		return result;
	}

	int readfd(){
		return read_.fd_;
	}

	Descriptor::State readState(){
		return read_.state_;
	}

	int writefd(){
		return write_.fd_;
	}

	Descriptor::State writeState(){
		return write_.state_;
	}

	size_t readCount(){ return read_.count_; }

	size_t writeCount(){ return write_.count_; }

	void closeRead(){
		read_.close();
	}

	void closeWrite(){
		write_.close();
	}

	void stdin(){
		read_.closeAndDup(0);
	}

	void stdout(){
		write_.closeAndDup(1);
	}

	void stderr(){
		write_.closeAndDup(2);
	}

	void nonblockingWrite(){
		long int oldFlags;

        if ((oldFlags=::fcntl(write_.fd_,F_GETFL))<0)
            throw std::runtime_error(std::string("Pipe::nonblockingWrite: unable to ::fcntl(pipefd_.data()[1],F_GETFL): ")+strerror(errno));
        if (::fcntl(write_.fd_,F_SETFL,oldFlags | O_NONBLOCK)<0)
            throw std::runtime_error(std::string("Pipe::nonblockingWrite: unable to ::fcntl(pipefd_.data()[1],F_SETFL,old_flags | O_NONBLOCK)")+strerror(errno));
	}

	void nonblockingRead(){
		long int oldFlags;

        if ((oldFlags=::fcntl(read_.fd_,F_GETFL))<0)
            throw std::runtime_error("Pipe::nonblockingRead: unable to ::fcntl(pipefd_.data()[0],F_GETFL)");
        if (::fcntl(read_.fd_,F_SETFL,oldFlags | O_NONBLOCK)<0)
            throw std::runtime_error("Pipe::nonblockingRead: unable to ::fcntl(pipefd_.data()[0],F_SETFL,old_flags | O_NONBLOCK)");
	}

	size_t read(char *buffer, size_t n){
	    //if (read1DescriptorState_==DescriptorState::CLOSED) return 0;

	    ssize_t result=::read(read_.fd_,buffer,n);
	    //read1DescriptorState_=DescriptorState::OPENED;

	    if (result<0){
			if (errno==EAGAIN){
				// read is non-blocking but pipe is empty
				return 0;
			}
			else
				throw std::runtime_error(std::string("Pipe::read: ")+strerror(errno));
	    }

	    read_.count_+=result;

	    //if (result==0 && getChildState()==ChildState::EXITED) close_read1();

	    return result;
	}

	std::string read(){
		char data[1024];

		ssize_t result=read(data,sizeof(data));
		return std::string(data,result);
	}

	ssize_t write(char const *data, size_t n){
		ssize_t result=::write(write_.fd_,data,n);
		// writeDescriptorState_=DescriptorState::OPENED;

		if (result<0)
			if (errno==EAGAIN){
				// write is non-blocking but pipe is full
				return 0;
			}
			else
				throw std::runtime_error(std::string("Pipe::write: ")+strerror(errno));
		else {
			write_.count_+=result;

			return result;
		}
	}

	std::string write(std::string const & s){
		ssize_t result=write(s.c_str(),s.size());

		return s.substr(result);
	}
};

#endif /* SRC_COMMON_PROCESS_PIPE_HPP_ */
