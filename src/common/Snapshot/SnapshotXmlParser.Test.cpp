/*
 * SnapshotXmlParser.Test.cpp
 *
 *  Created on: Apr 28, 2019
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Snapshot/SnapshotXmlParser.hpp"
#include "Xmltar/XmltarOptions.hpp"
#include "Xmltar/XmltarGlobals.hpp"

TEST(SnapshotXmlParser,PlaintextParse)
{
	SnapshotXmlParser snapshotParser;

	std::string xml=
			"<snapshot xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">"
				"<file name=\"../xmltar\">"
					"<event backup-time=\"99\" dump-level=\"4\" volume-number=\"0\" sha3-512=\"\"/>"
				"</file>"
					"<file name=\"../xmltar/.cproject\">"
						"<event backup-time=\"98\" dump-level=\"5\" volume-number=\"1\" sha3-512=\"1234\"/>"
				"</file>"
				"<file name=\"../xmltar/.git\">"
					"<event backup-time=\"97\" dump-level=\"6\" volume-number=\"2\" sha3-512=\"\"/>"
				"</file>"
				"<file name=\"../xmltar/.git/COMMIT_EDITMSG\">"
					"<event backup-time=\"96\" dump-level=\"7\" volume-number=\"3\" sha3-512=\"5678\"/>"
					"<event backup-time=\"95\" dump-level=\"8\" volume-number=\"4\" sha3-512=\"90\"/>"
				"</file>"
			"</snapshot>";

	snapshotParser.Parse(xml.c_str(),true);

	EXPECT_EQ(snapshotParser.fileEntries_.size(),4);
	EXPECT_EQ(snapshotParser.fileEntries_[2]->pathname_,"../xmltar/.git");
	EXPECT_EQ(snapshotParser.fileEntries_[0]->snapshotEvents_[0]->backupTime_,99);
	EXPECT_EQ(snapshotParser.fileEntries_[1]->snapshotEvents_[0]->dumpLevel_,5);
	EXPECT_EQ(snapshotParser.fileEntries_[2]->snapshotEvents_.size(),1);
	EXPECT_EQ(snapshotParser.fileEntries_[2]->snapshotEvents_[0]->volumeNumber_,2);
	EXPECT_EQ(snapshotParser.fileEntries_[3]->snapshotEvents_.size(),2);
	EXPECT_EQ(snapshotParser.fileEntries_[3]->snapshotEvents_[0]->sha3_512_,"5678");
	EXPECT_EQ(snapshotParser.fileEntries_[3]->snapshotEvents_[1]->sha3_512_,"90");

}
