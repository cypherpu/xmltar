/*

SnapshotXmlParser.Test.cpp
Copyright 2019-2020 David A. Betz
David.Betz.MD@gmail.com
Created on: Apr 28, 2019
Author: dbetz

This file is part of Xmltar.

Xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Xmltar.  If not, see <https://www.gnu.org/licenses/>.

 */

#include <gmock/gmock.h>

#include "Snapshot/SnapshotXmlParser.hpp"
#include "Xmltar/XmltarOptions.hpp"
#include "Xmltar/XmltarGlobals.hpp"

TEST(SnapshotXmlParser,PlaintextParse)
{
#if 0
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
