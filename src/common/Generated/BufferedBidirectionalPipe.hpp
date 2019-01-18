/*
 * BufferedBidirectionalPipe.hpp
 *
 *  Created on: Jan 15, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_BIDIRECTIONAL_PIPE_BUFFEREDBIDIRECTIONALPIPE_HPP_
#define SRC_COMMON_BIDIRECTIONAL_PIPE_BUFFEREDBIDIRECTIONALPIPE_HPP_

#include <deque>
#include <string>

#include "Bidirectional_Pipe.hpp"

class BufferedBidirectionalPipe : public Bidirectional_Pipe {
protected:
    std::deque<std::string> writeDeque_;
    std::deque<std::string> read1Deque_;
    std::deque<std::string> read2Deque_;

public:
	BufferedBidirectionalPipe()
		: Bidirectional_Pipe() {}
	BufferedBidirectionalPipe(const char *path, std::vector<char const *> argv)
    	: Bidirectional_Pipe(path, argv) {}

    void Open(const char *path, std::vector<char const *> argv){
    	Bidirectional_Pipe::Open(path,argv);
    }

    bool sss_Can_Read1(void){
        DEBUGCXX(debugcxx,"BufferedBidirectionalPipe::Can_Read1()");

        return read1Deque_.size()>0;
    }

    bool sss_Can_Read2(void){
        DEBUGCXX(debugcxx,"BufferedBidirectionalPipe::Can_Read2()");

        return read2Deque_.size()>0;
    }

    void sss_Write(std::string const & data){
        DEBUGCXX(debugcxx,"BufferedBidirectionalPipe::Write()");

        writeDeque_.push_back(data);
        queued_write_count+=data.size();
    }

    void sss_Write(char const *c, int n){
        DEBUGCXX(debugcxx,"Bidirectional_Pipe::QueueWrite()");

        writeDeque_.push_back(std::string(c,n));
        queued_write_count+=n;
    }

    std::string sss_Read1(){
    	std::string result;

    	if (read1Deque_.size()>0){
    		result=read1Deque_[0];
    		read1Deque_.pop_front();
    	}

    	pipeRead1Count_+=result.size();

    	return result;
    }

    std::string sss_Read2(){
    	std::string result;

    	if (read2Deque_.size()>0){
    		result=read2Deque_[0];
    		read2Deque_.pop_front();
    	}

    	pipeRead2Count_+=result.size();

    	return result;
    }

    void sss_writeClose(){
    	writeCloseWhenEmpty_=true;
    	if (writeDeque_.size()==0) close_write();
    }

    operator bool() {
    	// std::cerr << "Before Set_Child_Status()" << std::endl;
    	Set_Child_Status();
    	// std::cerr << "Before Select_Nonblocking()" << std::endl;
    	Select_Nonblocking();
    	// std::cerr << "writeDescriptorState_" << writeDescriptorState_ << std::endl;
    	// std::cerr << "read1DescriptorState_" << read1DescriptorState_ << std::endl;
    	// std::cerr << "read2DescriptorState_" << read2DescriptorState_ << std::endl;
    	// std::cerr << "sss_writeDeque_.size()=" << sss_writeDeque_.size() << std::endl;
    	// std::cerr << "sss_read1Deque_.size()=" << sss_read1Deque_.size() << std::endl;
    	// std::cerr << "sss_read2Deque_.size()=" << sss_read2Deque_.size() << std::endl;

    	if (read1DescriptorState_==DescriptorState::OPENED_READABLE){
    	    char buf[PIPE_BUF];

    	    ssize_t result=::read(child_stdout_to_parent_,buf,PIPE_BUF);

    	    if (result<0)
    	    	throw std::runtime_error("Bidirectional_Pipe::Read1: read error");

    	    // sss_dequeRead1Count_+=result;

    	    if (result==0 && getChildState()==ChildState::EXITED) close_read1();

    	    read1Deque_.push_back(std::string(buf,result));
    	}

    	if (read2DescriptorState_==DescriptorState::OPENED_READABLE){
    	    char buf[PIPE_BUF];

    	    ssize_t result=::read(child_stderr_to_parent_,buf,PIPE_BUF);

    	    if (result<0)
    	    	throw std::runtime_error("Bidirectional_Pipe::Read2: read error");

    	    // sss_dequeRead2Count_+=result;

    	    if (result==0 && getChildState()==ChildState::EXITED) close_read2();

    	    // std::cerr << "r2=" << result << " ";

    	    read2Deque_.push_back(std::string(buf,result));
    	}

    	if (writeDescriptorState_==DescriptorState::OPENED_WRITABLE){
    	    if (writeDeque_.size()>0){
    	        ssize_t result=::write(parent_to_child_stdin_,writeDeque_[0].data(),writeDeque_[0].length());

    	        if (result<0)
    	        	if (errno==EAGAIN){
    	        		// write is non-blocking but pipe is full
    	        	}
    	        	else
    	    			throw std::runtime_error("Bidirectional_Pipe::Write: write error");
    	        else {
    	    		write_count+=result;
    	    		writeDeque_[0]=writeDeque_[0].substr(result);
    	    		if (writeDeque_[0].size()==0) writeDeque_.pop_front();
    	        }
    	    }

    	    if (writeDeque_.size()==0 && writeCloseWhenEmpty_){
    	    	// std::cerr << "Bidirectional_Pipe::Write: closing write" << std::endl;
    	    	close_write();
    	    }
    	}
    	if (childState_==ChildState::EXITED &&
    		read1DescriptorState_==DescriptorState::CLOSED &&
    		read1Deque_.size()==0 &&
    		read2DescriptorState_==DescriptorState::CLOSED &&
    		read2Deque_.size()==0 &&
    		writeDescriptorState_==DescriptorState::CLOSED &&
    		writeDeque_.size()==0)
    		return false;

    	return true;
    }

    bool sss_Can_Write(void){
        DEBUGCXX(debugcxx,"Bidirectional_Pipe::Can_Write()");

        return writeDescriptorState_==DescriptorState::OPENED_WRITABLE;	// FIXME
    }
};

#endif /* SRC_COMMON_BIDIRECTIONAL_PIPE_BUFFEREDBIDIRECTIONALPIPE_HPP_ */
