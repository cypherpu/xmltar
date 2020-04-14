/*
 * Test.XmltarArchive.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: dbetz
 */

#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>

#include <gmock/gmock.h>

#include "Xmltar/XmltarArchive.hpp"
#include "Utilities/CompareFiles.hpp"
#include "Utilities/TemporaryFile.hpp"

std::string exePath("/home/dbetz/git/xmltar/bazel-bin/xmltar");
std::string workingDirPath("/home/dbetz/git");
std::string targetBasePath("Utilities");

void Execute(std::vector<std::string> const & argv){
	std::vector<std::string> tmp;

	for(size_t i=0; i<argv.size(); ++i)
		tmp.push_back(argv[i]+'\0');

	std::vector<char *> tmp2;

	for(size_t i=0; i<tmp.size(); ++i)
		tmp2.push_back(tmp[i].data());
	tmp2.push_back(nullptr);

	const std::vector<char *> tmp3(tmp2);

	pid_t pid=fork();
	if (!pid)
		execv(argv[0].c_str(),tmp3.data());
	if (wait(nullptr)!=pid)
			throw std::runtime_error("Execute: wait return != pid");
}

TEST(XmltarTest,IsPaddingTrailer)
{
	std::string cleartext("<padding>0123456789abcdef</padding></xmltar>");
	{
		ASSERT_TRUE(XmltarArchive::IsPaddingTrailer(cleartext));
	}
}

bool CreateAndVerifyArchive(std::filesystem::path workingDirPath,
							std::filesystem::path tempPath,
							std::vector<std::string> createArchive,
							std::vector<std::string> extractArchive){
	std::filesystem::current_path(workingDirPath);

	Execute(createArchive);

	std::filesystem::current_path(tempPath);

	Execute(std::vector<std::string>{"/bin/rm",tempPath.string()+"/Utilities"});
	Execute(extractArchive);

	std::filesystem::current_path(workingDirPath);

	return CompareFiles(targetBasePath,tempPath/targetBasePath);
}

TEST(XmltarTest,SingleArchive)
{
	std::filesystem::path tempPath(TemporaryDir("/tmp/xmltar_test_XXXXXX"));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose",
						"--file",(tempPath/targetBasePath).string()+".id.id.id.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose",
						"--file",(tempPath/targetBasePath).string()+".id.id.id.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--gzip",
						"--file",(tempPath/targetBasePath).string()+".id.id.gzip.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--gzip",
						"--file",(tempPath/targetBasePath).string()+".id.id.gzip.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--zstd",
						"--file",(tempPath/targetBasePath).string()+".id.id.zstd.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--zstd",
						"--file",(tempPath/targetBasePath).string()+".id.id.zstd.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--member-gzip",
						"--file",(tempPath/targetBasePath).string()+".id.gzip.id.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--member-gzip",
						"--file",(tempPath/targetBasePath).string()+".id.gzip.id.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--member-zstd",
						"--file",(tempPath/targetBasePath).string()+".id.zstd.id.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--member-zstd",
						"--file",(tempPath/targetBasePath).string()+".id.zstd.id.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--file-gzip",
						"--file",(tempPath/targetBasePath).string()+".gzip.id.id.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--file-gzip",
						"--file",(tempPath/targetBasePath).string()+".gzip.id.id.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--file-zstd",
						"--file",(tempPath/targetBasePath).string()+".zstd.id.id.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--file-zstd",
						"--file",(tempPath/targetBasePath).string()+".zstd.id.id.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--member-gzip","--gzip",
						"--file",(tempPath/targetBasePath).string()+".id.gzip.gzip.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--member-gzip","--gzip",
						"--file",(tempPath/targetBasePath).string()+".id.gzip.gzip.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--member-zstd","--zstd",
						"--file",(tempPath/targetBasePath).string()+".id.zstd.zstd.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--member-zstd","--zstd",
						"--file",(tempPath/targetBasePath).string()+".id.zstd.zstd.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--file-gzip","--gzip",
						"--file",(tempPath/targetBasePath).string()+".gzip.id.gzip.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--file-gzip","--gzip",
						"--file",(tempPath/targetBasePath).string()+".gzip.id.gzip.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--file-zstd","--zstd",
						"--file",(tempPath/targetBasePath).string()+".zstd.id.zstd.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--file-zstd","--zstd",
						"--file",(tempPath/targetBasePath).string()+".zstd.id.zstd.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--file-gzip","--member-gzip",
						"--file",(tempPath/targetBasePath).string()+".gzip.gzip.id.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--file-gzip","--member-gzip",
						"--file",(tempPath/targetBasePath).string()+".gzip.gzip.id.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--file-zstd","--member-zstd",
						"--file",(tempPath/targetBasePath).string()+".zstd.zstd.id.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--file-zstd","--member-zstd",
						"--file",(tempPath/targetBasePath).string()+".zstd.zstd.id.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--file-gzip","--member-gzip","--gzip",
						"--file",(tempPath/targetBasePath).string()+".gzip.gzip.gzip.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--file-gzip","--member-gzip","--gzip",
						"--file",(tempPath/targetBasePath).string()+".gzip.gzip.gzip.xmltar"}));

	ASSERT_TRUE(
		CreateAndVerifyArchive(
			workingDirPath,
			tempPath,
			std::vector<std::string>{exePath,"--create","--verbose","--file-zstd","--member-zstd","--zstd",
						"--file",(tempPath/targetBasePath).string()+".zstd.zstd.zstd.xmltar",
						targetBasePath},
			std::vector<std::string>{exePath,"--extract","--verbose","--file-zstd","--member-zstd","--zstd",
						"--file",(tempPath/targetBasePath).string()+".zstd.zstd.zstd.xmltar"}));
}
