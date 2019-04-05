/*
 * XmltarOptions.Test.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Options/XmltarOptions.hpp"

TEST(XmltarTest,Options1)
{
	{
		std::vector<const char *> options{"./xmltar","cf","foo"};
		XmltarOptions opts;
		opts.ProcessOptions(options.size(), &options[0]);
		EXPECT_EQ(opts.operation_,XmltarOptions::CREATE);
		EXPECT_TRUE(opts.base_xmltar_file_name_ && opts.base_xmltar_file_name_.get()=="foo");
		EXPECT_FALSE(opts.sourceFileGlobs_.size());
		spdlog::debug(opts.toXMLString());
	}
}

TEST(XmltarTest,Options2)
{
	{
		std::vector<const char *> options{"./xmltar","-cfz","foo"};
		XmltarOptions opts;
		opts.ProcessOptions(options.size(), &options[0]);
		EXPECT_TRUE(opts.operation_ && opts.operation_==XmltarOptions::CREATE);
		EXPECT_TRUE(opts.base_xmltar_file_name_ && opts.base_xmltar_file_name_.get()=="z");
		EXPECT_TRUE(opts.sourceFileGlobs_.size()==1 && opts.sourceFileGlobs_[0]=="foo");
		spdlog::debug(opts.toXMLString());
	}
}
