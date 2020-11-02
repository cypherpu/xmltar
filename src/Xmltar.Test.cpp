/*
 * Tar.Test.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include <spdlog/spdlog.h>

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	spdlog::set_level(spdlog::level::err);
	spdlog::set_pattern("[%Y-%m-%d] [%H:%M:%S %z] [%l] [%P] %v");
	//::testing::GTEST_FLAG(filter) = "tA.*";
	// ::testing::GTEST_FLAG(filter) = "SnapshotXmlParser.*";
	//::testing::GTEST_FLAG(filter) = "Snapshot.MergeSnapshotFilesHelper";

	return RUN_ALL_TESTS();
}

