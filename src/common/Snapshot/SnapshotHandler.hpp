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

#include <xercesc/sax2/DefaultHandler.hpp>

class Snapshot;

class SnapshotHandler : public xercesc::DefaultHandler
{
	Snapshot & snapshot_;
	std::vector<std::string> elementNameStack_;
	std::vector<std::vector<XMLCh>> characterDataStack_;

	std::vector<std::string> optionsStack_;
public:
	SnapshotHandler(Snapshot & snapshot)
		: snapshot_(snapshot){}
	virtual ~SnapshotHandler(){}

	void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs);
	void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
	void characters(const XMLCh* const chars, const XMLSize_t length);
	void fatalError(const xercesc::SAXParseException&);

	std::vector<char const *> optionsArgv();
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTHANDLER_HPP_ */
