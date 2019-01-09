/*
 * SnapshotHandler.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: dbetz
 */

#include <iostream>

#include <boost/lexical_cast.hpp>

#include "Snapshot/SnapshotHandler.hpp"
#include "Snapshot/Snapshot.hpp"

void SnapshotHandler::startElement(const char * const uri, const char * const localname, const char * const qname, const char **attrs){
}

void SnapshotHandler::endElement(const char * const uri, const char * const localname, const char * const qname){
}

void SnapshotHandler::characters(const char * const chars, const size_t length){
}

std::vector<char const *> SnapshotHandler::optionsArgv(){
	std::vector<char const *> result;

	result.push_back("./xmltar");
	for(auto & i : optionsStack_)
		result.push_back(i.c_str());

	return result;
}

