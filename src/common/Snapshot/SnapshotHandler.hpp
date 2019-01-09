/*
 * SnapshotHandler.hpp
 *
 *  Created on: Nov 20, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTHANDLER_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTHANDLER_HPP_

#include <string>
#include <vector>

class Snapshot;

class SnapshotHandler
{
	Snapshot & snapshot_;
	std::vector<std::string> elementNameStack_;
	std::vector<std::vector<char>> characterDataStack_;

	std::vector<std::string> optionsStack_;
public:
	SnapshotHandler(Snapshot & snapshot)
		: snapshot_(snapshot){}
	virtual ~SnapshotHandler(){}

	void startElement(const char * const uri, const char * const localname, const char * const qname, const char **attrs);
	void endElement(const char * const uri, const char * const localname, const char * const qname);
	void characters(const char * const chars, const size_t length);

	std::vector<char const *> optionsArgv();
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTHANDLER_HPP_ */
