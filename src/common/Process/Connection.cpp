/*
 * Connection.cpp
 *
 *  Created on: Feb 9, 2019
 *      Author: dbetz
 */

#include "Process/Connection.hpp"

std::ostream & operator<<(std::ostream & os, EndPointAction a){
	if (a==EndPointAction::CLOSE) os << "CLOSE";
	else if (a==EndPointAction::STDIN) os << "STDIN";
	else if (a==EndPointAction::STDOUT) os << "STDOUT";
	else if (a==EndPointAction::STDERR) os << "STDERR";
	else if (a==EndPointAction::BLOCKING_WRITE) os << "BLOCKING_WRITE";
	else if (a==EndPointAction::BLOCKING_READ) os << "BLOCKING_READ";
	else if (a==EndPointAction::NONBLOCKING_WRITE) os << "NONBLOCKING_WRITE";
	else if (a==EndPointAction::NONBLOCKING_READ) os << "NONBLOCKING_READ";
	else throw std::runtime_error("operator<<: unrecognized EndPointAction");

	return os;
}

void launch(std::vector<Connection> connections){
	std::set<Process *> processes;
	std::set<Pipe *> pipes;

	for(auto & i : connections)
		processes.insert(i.process());

	for(auto & i : connections)
		pipes.insert(i.pipe());

	for(auto i : pipes){
		if (i->pipe()<0)
			throw std::runtime_error(std::string("launch: pipe error: ")+strerror(errno));

		// std::cerr << i->readfd() << " " << i->writefd() << std::endl;
	}

	for(auto i : processes){
		if (i==nullptr) continue;
		//::sleep(1);
		std::set<Pipe *> retainedChildPipes;
		if (!i->fork()){	// child
			// ::sleep(1);
			for(auto j : connections){
				if (j.process()==i){
					// std::cerr << "childAction: " << i->nickname() << " ";
					j.endPointAction();
					retainedChildPipes.insert(j.pipe());
				}
			}

			std::set<Pipe *> discardedPipes;
			std::set_difference(pipes.begin(),pipes.end(),
								retainedChildPipes.begin(),retainedChildPipes.end(),
								std::inserter(discardedPipes,discardedPipes.end()));

			for(auto j : discardedPipes){
				j->closeRead();
				j->closeWrite();
			}

			i->exec();
		}
	}

	// ::sleep(3);
	for(auto i : connections)
		if (i.process()==nullptr)
			i.endPointAction();
}
