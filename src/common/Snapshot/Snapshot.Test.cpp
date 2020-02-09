/*
 * Snapshot.Test.cpp
 *
 *  Created on: May 4, 2019
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Snapshot/Snapshot.hpp"
#include "Utilities/TemporaryFile.hpp"
#include "Utilities/Identity.hpp"
#include "Compressors/Compressor.hpp"

TEST(Snapshot,MergeSnapshotFilesHelper)
{
	std::filesystem::path testDirPath=TemporaryDir(std::filesystem::temp_directory_path() / "Test_dir_XXXXXX");
	std::filesystem::path testFilePath1=testDirPath / "Test1.xml";
	std::filesystem::path testFilePath2=testDirPath / "Test2.xml";
	std::filesystem::path testFilePath3=testDirPath / "Test3.xml";
	std::filesystem::path targetPath=testDirPath / "Target.xml";

	std::ofstream testOfs1(testFilePath1);
	std::ofstream testOfs2(testFilePath2);
	std::ofstream testOfs3(testFilePath3);

	testOfs1 <<
			"<snapshot xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">\n"
				"\t<file name=\"../xmltar\">\n"
					"\t\t<event backup-time=\"1\" dump-level=\"101\" volume-number=\"201\" sha3-512=\"\"/>\n"
				"\t</file>\n"
				"\t<file name=\"../xmltar/.cproject\">\n"
					"\t\t<event backup-time=\"2\" dump-level=\"102\" volume-number=\"202\" sha3-512=\"302\"/>\n"
				"\t</file>\n"
			"</snapshot>";

	testOfs2 <<
			"<snapshot xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">\n"
				"\t<file name=\"../xmltar\">\n"
					"\t\t<event backup-time=\"3\" dump-level=\"103\" volume-number=\"203\" sha3-512=\"\"/>\n"
				"\t</file>\n"
				"\t<file name=\"../xmltar/.cproject\">\n"
					"\t\t<event backup-time=\"4\" dump-level=\"104\" volume-number=\"204\" sha3-512=\"304\"/>\n"
				"\t</file>\n"
			"</snapshot>";

	testOfs3 <<
			"<snapshot xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">\n"
				"\t<file name=\"../xmltar\">\n"
					"\t\t<event backup-time=\"5\" dump-level=\"105\" volume-number=\"205\" sha3-512=\"\"/>\n"
				"\t</file>\n"
				"\t<file name=\"../xmltar/.cproject\">\n"
					"\t\t<event backup-time=\"6\" dump-level=\"106\" volume-number=\"206\" sha3-512=\"306\"/>\n"
				"\t</file>\n"
			"</snapshot>";

	testOfs1.close();
	testOfs2.close();
	testOfs3.close();

	std::vector<std::filesystem::path> sourcePaths { testFilePath1,testFilePath2,testFilePath3 };
	std::shared_ptr<CompressorInterface> compression=std::make_shared<Compressor<Identity>>();
	// MergeSnapshotFilesHelper(sourcePaths, targetPath, compression);

}
