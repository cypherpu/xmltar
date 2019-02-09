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

class BufferedBidirectionalPipe {
private:
	Bidirectional_Pipe p_;
protected:
    std::deque<std::string> writeDeque_;
    std::deque<std::string> read1Deque_;
    std::deque<std::string> read2Deque_;

    size_t bufferedWriteCount_;
    bool close_write_when_empty_;

public:
	BufferedBidirectionalPipe()
		: p_(), writeDeque_(),read1Deque_(), read2Deque_(), bufferedWriteCount_(0), close_write_when_empty_(false) {}
	BufferedBidirectionalPipe(const char *path, std::vector<char const *> argv)
    	: p_(path, argv), writeDeque_(),read1Deque_(), read2Deque_(), bufferedWriteCount_(0), close_write_when_empty_(false) {}

    void Open(const char *path, std::vector<char const *> argv){
    	p_.Open(path,argv);
    }

    void Block(unsigned int microseconds){
    	p_.Select_Blocking(microseconds);
    }

    bool Buffered_Can_Read1(void){
        DEBUGCXX(debugcxx,"BufferedBidirectionalPipe::Can_Read1()");

        return read1Deque_.size()>0;
    }

    bool Buffered_Can_Read2(void){
        DEBUGCXX(debugcxx,"BufferedBidirectionalPipe::Can_Read2()");

        return read2Deque_.size()>0;
    }

    void Buffered_Write(std::string const & data){
        DEBUGCXX(debugcxx,"BufferedBidirectionalPipe::Write()");

        if (close_write_when_empty_)
        	throw std::logic_error("BufferedBidirectionalPipe::Buffered_Write(std::string const & data): attempt to write after close");

        writeDeque_.push_back(data);
        bufferedWriteCount_+=data.size();
    }

    void Buffered_Write(char const *c, int n){
        DEBUGCXX(debugcxx,"BufferedBidirectionalPipe::Buffered_Write(char const *c, int n)");

        if (close_write_when_empty_)
        	throw std::logic_error("BufferedBidirectionalPipe::Buffered_Write(char const *c, int n): attempt to write after close");

        writeDeque_.push_back(std::string(c,n));
        bufferedWriteCount_+=n;
    }

    bool Can_Read1(){
    	return p_.Can_Read1();
    }

    bool Can_Read2(){
    	return p_.Can_Read2();
    }

    bool Can_Write(){
    	return p_.Can_Write();
    }


    std::string Buffered_Read1(){
    	std::string result;

    	if (read1Deque_.size()>0){
    		result=read1Deque_[0];
    		read1Deque_.pop_front();
    	}

    	return result;
    }

    std::string Buffered_Read2(){
    	std::string result;

    	if (read2Deque_.size()>0){
    		result=read2Deque_[0];
    		read2Deque_.pop_front();
    	}

    	return result;
    }

    void Buffered_close_write(){
    	close_write_when_empty_=true;
    	if (writeDeque_.size()==0) p_.close_write();
    }

    operator bool() {
    	p_.Set_Child_Status();
    	p_.Select_Nonblocking();

    	if (p_.read1DescriptorState_==Bidirectional_Pipe::DescriptorState::OPENED_READABLE){
    	    char buf[PIPE_BUF];

        	ssize_t result=p_.read1(buf,PIPE_BUF);

        	if (result>0) read1Deque_.push_back(std::string(buf,result));
    	}

    	if (p_.read2DescriptorState_==Bidirectional_Pipe::DescriptorState::OPENED_READABLE){
    	    char buf[PIPE_BUF];

    	    ssize_t result=p_.read2(buf,PIPE_BUF);

    	    if (result>0) read2Deque_.push_back(std::string(buf,result));
    	}

    	if (p_.writeDescriptorState_==Bidirectional_Pipe::DescriptorState::OPENED_WRITABLE){
    	    if (writeDeque_.size()>0){
    	        ssize_t result=p_.write(writeDeque_[0].data(),writeDeque_[0].length());

    	        if (result<0)
    	        	if (errno==EAGAIN){
    	        		// write is non-blocking but pipe is full
    	        	}
    	        	else
    	    			throw std::runtime_error("Bidirectional_Pipe::Write: write error");
    	        else {
    	    		writeDeque_[0]=writeDeque_[0].substr(result);
    	    		if (writeDeque_[0].size()==0) writeDeque_.pop_front();
    	        }
    	    }

    	    if (writeDeque_.size()==0 && close_write_when_empty_){
    	    	p_.close_write();
    	    }
    	}
    	if (p_.childState_==Bidirectional_Pipe::ChildState::EXITED &&
    		p_.read1DescriptorState_==Bidirectional_Pipe::DescriptorState::CLOSED &&
    		read1Deque_.size()==0 &&
			p_.read2DescriptorState_==Bidirectional_Pipe::DescriptorState::CLOSED &&
    		read2Deque_.size()==0 &&
			p_.writeDescriptorState_==Bidirectional_Pipe::DescriptorState::CLOSED &&
    		writeDeque_.size()==0)
    		return false;

    	return true;
    }

    size_t Read1Count(){
    	return p_.pipeRead1Count_;
    }

    size_t Read2Count(){
    	return p_.pipeRead2Count_;
    }

    size_t WriteCount(){
    	return p_.pipeWriteCount();
    }

    size_t BufferedWriteCount(){
    	return bufferedWriteCount_;
    }

    int ExitStatus(){
    	return p_.ExitStatus();
    }

    size_t WriteBufferSize(){
    	size_t result=0;

    	for(auto & i : writeDeque_)
    		result+=i.size();

    	return result;
    }

    size_t Read1BufferSize(){
    	size_t result=0;

    	for(auto & i : read1Deque_)
    		result+=i.size();

    	return result;
    }

    size_t Read2BufferSize(){
    	size_t result=0;

    	for(auto & i : read2Deque_)
    		result+=i.size();

    	return result;
    }
};

#endif /* SRC_COMMON_BIDIRECTIONAL_PIPE_BUFFEREDBIDIRECTIONALPIPE_HPP_ */
