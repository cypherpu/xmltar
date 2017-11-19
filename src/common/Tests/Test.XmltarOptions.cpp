/*
 * XmltarOptions.Test.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Xmltar/XmltarOptions.hpp"

TEST(XmltarTest,Options1)
{
	{
		std::vector<const char *> options{"./xmltar","cf","foo"};
		XmltarOptions opts;
		opts.ProcessOptions(options.size(), &options[0]);
		EXPECT_EQ(opts.operation_,XmltarOptions::CREATE);
		EXPECT_TRUE(opts.base_xmltar_file_name_=="foo");
		EXPECT_TRUE(opts.source_files_.size()==0);
	}
}

TEST(XmltarTest,Options2)
{
	{
		std::vector<const char *> options{"./xmltar","-cfz","foo"};
		XmltarOptions opts;
		opts.ProcessOptions(options.size(), &options[0]);
		EXPECT_EQ(opts.operation_,XmltarOptions::CREATE);
		EXPECT_TRUE(opts.base_xmltar_file_name_=="z");
		EXPECT_TRUE(opts.source_files_.size()==1 && opts.source_files_[0]=="foo");
	}
}
