/*
 * Snapshot.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#include <iostream>

#include "Snapshot/Snapshot.hpp"

Snapshot::Snapshot(){
}

Snapshot::Snapshot(std::string const & xmlFile){
	Load(xmlFile);
}

void Snapshot::Load(std::string const & xmlFile){
}

void Snapshot::dump(std::ostream & os){
	for(auto & i : fileEntries_){
		os << "\t<file name=\"" << i.pathname_.string() << "\">\n";
		for(auto & j : i.snapshotEvents_){
			os << "\t\t" << j << "\n";
		}
	}
}
