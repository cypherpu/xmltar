/*
 * Test.XmltarArchive.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Xmltar/XmltarArchive.hpp"

TEST(XmltarTest,IsPaddingTrailer)
{
	std::string cleartext("<padding>0123456789abcdef</padding></xmltar>");
	{
		ASSERT_TRUE(XmltarArchive::IsPaddingTrailer(cleartext));
	}
}
