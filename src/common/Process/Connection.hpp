/*
 * Connection.hpp
 *
 *  Created on: Feb 9, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_PROCESS_CONNECTION_HPP_
#define SRC_COMMON_PROCESS_CONNECTION_HPP_

#include "Process/Pipe.hpp"
#include "Process/Process.hpp"

enum class EndPointAction { CLOSE, STDIN, STDOUT, STDERR,BLOCKING_WRITE, BLOCKING_READ, NONBLOCKING_WRITE, NONBLOCKING_READ };

std::ostream & operator<<(std::ostream & os, EndPointAction a);

class Connection {
	Process *process_;
	Pipe *pipe_;
	EndPointAction readEnd_;
	EndPointAction writeEnd_;
public:
	Connection(Process & process, Pipe & pipe, EndPointAction readEnd, EndPointAction writeEnd)
		: process_(&process), pipe_(&pipe), readEnd_(readEnd), writeEnd_(writeEnd) {
	}

	Connection(Pipe & pipe, EndPointAction readEnd, EndPointAction writeEnd)
		: process_(nullptr), pipe_(&pipe), readEnd_(readEnd), writeEnd_(writeEnd) {
	}

	Process *process(){ return process_; }
	Pipe *pipe(){ return pipe_; }
	void endPointAction(){
		// std::cerr << pipe_->readfd() << " " << readEnd_ << " " <<  pipe_->writefd() << " " << writeEnd_ << std::endl;

		if (readEnd_==EndPointAction::CLOSE)
			pipe_->closeRead();
		// else if (readEnd_==EndPointAction::BLOCKING_READ)
		//	blockingRead();
		else if (readEnd_==EndPointAction::NONBLOCKING_READ)
			pipe_->nonblockingRead();
		else if (readEnd_==EndPointAction::STDIN)
			pipe_->stdin();

		if (writeEnd_==EndPointAction::CLOSE)
			pipe_->closeWrite();
		// else if (parentWriteEnd_==EndPointAction::BLOCKING_WRITE)
		//	blockingRead();
		else if (writeEnd_==EndPointAction::NONBLOCKING_WRITE)
			pipe_->nonblockingWrite();
		else if (writeEnd_==EndPointAction::STDOUT)
			pipe_->stdout();
		else if (writeEnd_==EndPointAction::STDERR)
			pipe_->stderr();
	}

};

void launch(std::vector<Connection> connections);

#endif /* SRC_COMMON_PROCESS_CONNECTION_HPP_ */
