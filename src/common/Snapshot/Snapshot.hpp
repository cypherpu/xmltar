/*
 * Snapshot.hpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_

#include <vector>

#include <xercesc/sax2/DefaultHandler.hpp>

#include "Snapshot/SnapshotOptions.hpp"
#include "Snapshot/SnapshotFileEntry.hpp"

class Snapshot {
	class SnapshotHandler : public xercesc::DefaultHandler
	{
		Snapshot & snapshot_;
		std::vector<std::string> elementNameStack_;
		std::vector<std::vector<XMLCh>> characterDataStack_;
	public:
		SnapshotHandler(Snapshot & snapshot)
			: snapshot_(snapshot){}
		virtual ~SnapshotHandler(){}

		void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs);
		void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
		void characters(const XMLCh* const chars, const XMLSize_t length);
		void fatalError(const xercesc::SAXParseException&);
	};

public:
	SnapshotOptions options_;
	std::vector<SnapshotFileEntry> fileEntries_;

	Snapshot();
	Snapshot(std::string const & xmlFile);
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOT_HPP_ */
