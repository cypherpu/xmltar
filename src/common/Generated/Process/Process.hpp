/*
 * Process.hpp
 *
 *  Created on: Feb 9, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_PROCESS_PROCESS_HPP_
#define SRC_COMMON_PROCESS_PROCESS_HPP_

#include <sys/wait.h>		// waitpid
#include <string.h>			// strerror

#include <iostream>
#include <string>
#include <set>
#include <vector>

#include <boost/optional.hpp>

class Process {
	std::string path_;
	std::vector<std::string> args_;
	std::string nickname_;

	boost::optional<pid_t> pid_;
public:
	Process()
		: path_(), args_(), nickname_(), pid_() {}

	Process(char const *path, std::vector<char const *> args, char const *nickname="")
		: path_(path), nickname_(nickname), pid_(){
		for(auto i : args)
			args_.push_back(i);
	}

	void Initialize(char const *path, std::vector<char const *> args, char const *nickname=""){
		path_=path;
		nickname_=nickname;
		for(auto i : args)
			args_.push_back(i);
	}

	pid_t fork(){
		if ((pid_=::fork()).get()==-1)
			throw std::runtime_error("Process::fork: failed fork");
		return pid_.get();
	}

	void exec(){
		std::vector<char const *> argv;

		for(auto & i : args_)
			argv.push_back(i.c_str());
		argv.push_back(nullptr);

		::execvp(path_.c_str(),const_cast<char * const *>(argv.data()));
	}

	explicit operator bool(){
		int status;
	    int waitpid_result=waitpid(pid_.get(),&status,WNOHANG);

	    if (waitpid_result<0)
	        throw std::runtime_error(std::string("Process::operator bool(): ")+strerror(errno));
	    else if (waitpid_result==0){
	    	// no change in child state
	    	return true;
	    }
	    else if (waitpid_result==pid_.get()){
	    	//std::cerr << " EXITED ";
	    	// childState_=ChildState::EXITED;
	        if (!WIFEXITED(status))
	            throw std::runtime_error(std::string("Process::operator bool(): ")+strerror(errno));
	        // exit_status_=WEXITSTATUS(status);
	        return false;
	    }
	    else
	    	throw std::runtime_error(std::string("Pipe::operator bool(): ")+strerror(errno));
	}

	std::string nickname(){ return nickname_; }
};

#endif /* SRC_COMMON_PROCESS_PROCESS_HPP_ */
