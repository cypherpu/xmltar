/*
 * SnapshotOptions.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#include "Snapshot/SnapshotOptionsParser.hpp"

bool ParseSnapshotOptions(std::string input, unsigned int year, SnapshotOptions & snapshotOptions){
	  typedef std::string::const_iterator iterator_type;
	  std::string::const_iterator iter = input.begin();
	  std::string::const_iterator end = input.end();
	  SnapshotOptionsParser<iterator_type> SnapshotOptionsParser_p;

	  bool r = parse(iter, end, SnapshotOptionsParser_p(year), snapshotOptions );

	  if (iter==end) return true;

	  return false;
}



