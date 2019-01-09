/*
 * Snapshot.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#include <iostream>

#include "Snapshot/Snapshot.hpp"
#include "Snapshot/SnapshotHandler.hpp"

Snapshot::Snapshot(){
}

Snapshot::Snapshot(std::string const & xmlFile){
	Load(xmlFile);
}

void Snapshot::Load(std::string const & xmlFile){
}
