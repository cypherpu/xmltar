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

std::string exePath("/home/dbetz/git/xmltar/bazel-bin/xmltar");
std::string workingDirPath("/home/dbetz/git");
std::string targetBasePath("Utilities");

void Execute(std::string exePath, std::vector<std::string> const & argv){
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
		execv(exePath.c_str(),tmp3.data());
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

TEST(XmltarTest,SingleArchive)
{
	std::filesystem::path startingWorkingDirPath=std::filesystem::current_path();
	std::filesystem::current_path(workingDirPath);

	Execute(exePath,
			std::vector<std::string>{exePath,"--create","--verbose",
			"--file","/tmp/"+targetBasePath+".xmltar",
			targetBasePath});

	std::filesystem::current_path("/tmp");

	Execute(exePath,
			std::vector<std::string>{exePath,"--extract","--verbose",
			"--file","/tmp/"+targetBasePath+".xmltar"});

	std::filesystem::current_path(workingDirPath);
	ASSERT_TRUE(CompareFiles(targetBasePath,"/tmp/"+targetBasePath));
}
