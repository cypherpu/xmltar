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
					"<event backup-time=\"99\" dump-level=\"4\" action=\"add\" starting-volume-name=\"0\" modification-time=\"314\" size=\"141\" sha3-512=\"\"/>"
				"</file>"
					"<file name=\"../xmltar/.cproject\">"
						"<event backup-time=\"98\" dump-level=\"5\" action=\"add\" starting-volume-name=\"1\" modification-time=\"314\" size=\"141\" sha3-512=\"1234\"/>"
				"</file>"
				"<file name=\"../xmltar/.git\">"
					"<event backup-time=\"97\" dump-level=\"6\" action=\"add\" starting-volume-name=\"2\" modification-time=\"314\" size=\"141\" sha3-512=\"\"/>"
				"</file>"
				"<file name=\"../xmltar/.git/COMMIT_EDITMSG\">"
					"<event backup-time=\"96\" dump-level=\"7\" action=\"add\" starting-volume-name=\"3\" modification-time=\"314\" size=\"141\" sha3-512=\"5678\"/>"
					"<event backup-time=\"95\" dump-level=\"8\" action=\"add\" starting-volume-name=\"4\" modification-time=\"314\" size=\"141\" sha3-512=\"90\"/>"
				"</file>"
			"</snapshot>";

	snapshotParser.Parse(xml.c_str(),true);
#if 0
	EXPECT_EQ(snapshotParser.fileEntries_.size(),4);
	// EXPECT_EQ(snapshotParser.fileEntries_[2]->pathname_,"../xmltar/.git");
	EXPECT_EQ(snapshotParser.fileEntries_[0]->snapshotEvents_[0]->backupTime_,99);
	EXPECT_EQ(snapshotParser.fileEntries_[1]->snapshotEvents_[0]->dumpLevel_,5);
	EXPECT_EQ(snapshotParser.fileEntries_[2]->snapshotEvents_.size(),1);
	EXPECT_EQ(snapshotParser.fileEntries_[2]->snapshotEvents_[0]->startingVolumeName_,2);
	EXPECT_EQ(snapshotParser.fileEntries_[3]->snapshotEvents_.size(),2);
	EXPECT_EQ(snapshotParser.fileEntries_[3]->snapshotEvents_[0]->sha3_512_,"5678");
	EXPECT_EQ(snapshotParser.fileEntries_[3]->snapshotEvents_[1]->sha3_512_,"90");
#endif
}
