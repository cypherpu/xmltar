/*
 * Tar.Test.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: dbetz
 */

#include <gmock/gmock.h>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "tA.*";
  return RUN_ALL_TESTS();
}

