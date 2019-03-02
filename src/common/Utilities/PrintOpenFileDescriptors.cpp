/*
 * PrintOpenFileDescriptors.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: dbetz
 */

#include <sys/types.h>
#include <unistd.h>

#include <ios>
#include <iomanip>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <spdlog/spdlog.h>

void PrintOpenFileDescriptors(){
	pid_t pid=getpid();

	std::ostringstream oss;

	oss << "/proc/" << pid << "/fd";

	std::array<int,20> descriptors;
	std::array<int,20> flags;
	std::array<int,20> fdflags;

	for(size_t i=0; i<descriptors.size(); ++i){
		descriptors[i]=0;
		flags[i]=0;
	}

	for(auto & p : boost::filesystem::directory_iterator(oss.str()) ){
		int descriptor=boost::lexical_cast<int>(p.path().leaf().string());
		descriptors[descriptor]=1;
        if ((flags[descriptor]=::fcntl(descriptor,F_GETFL))<0)
            throw std::runtime_error(std::string("PrintOpenFileDescriptors: unable to ::fcntl(,F_GETFL): ")+strerror(errno));
        if ((fdflags[descriptor]=::fcntl(descriptor,F_GETFD))<0)
            throw std::runtime_error(std::string("PrintOpenFileDescriptors: unable to ::fcntl(,F_GETFD): ")+strerror(errno));
	}

	std::ostringstream osss;
	osss << "Open descriptors: ";
	for(size_t i=0; i<descriptors.size(); ++i)
		if (descriptors[i]==0)
			osss << "--   ";
		else {
			osss << std::setw(2) << std::right << i;

	        if (fdflags[i] & FD_CLOEXEC)
	        	osss << "!";
	        else osss << " ";

	        if (flags[i] & O_NONBLOCK)
	        	osss << "#";
	        else osss << " ";

	        osss << "  ";
		}

	osss << std::endl;

	spdlog::debug(osss.str());
}

