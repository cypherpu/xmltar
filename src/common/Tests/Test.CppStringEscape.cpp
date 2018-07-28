/*
 * CppStringEscape.Test.cpp
 *
 *  Created on: Jul 25, 2018
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Utilities/CppStringEscape.hpp"

TEST(CppStringEscape,IsXmlChar)
{
	ASSERT_FALSE(IsXmlChar('\x0'));
	ASSERT_FALSE(IsXmlChar('\x1'));
	ASSERT_FALSE(IsXmlChar('\x2'));
	ASSERT_FALSE(IsXmlChar('\x3'));
	ASSERT_FALSE(IsXmlChar('\x4'));
	ASSERT_FALSE(IsXmlChar('\x5'));
	ASSERT_FALSE(IsXmlChar('\x6'));
	ASSERT_FALSE(IsXmlChar('\x7'));
	ASSERT_FALSE(IsXmlChar('\x8'));
	ASSERT_TRUE(IsXmlChar('\x9'));
	ASSERT_TRUE(IsXmlChar('\xA'));
	ASSERT_FALSE(IsXmlChar('\xB'));
	ASSERT_FALSE(IsXmlChar('\xC'));
	ASSERT_FALSE(IsXmlChar('\xD'));
	ASSERT_FALSE(IsXmlChar('\xE'));
	ASSERT_FALSE(IsXmlChar('\xF'));
	ASSERT_FALSE(IsXmlChar('\x10'));
	ASSERT_FALSE(IsXmlChar('\x11'));
	ASSERT_FALSE(IsXmlChar('\x12'));
	ASSERT_FALSE(IsXmlChar('\x13'));
	ASSERT_TRUE(IsXmlChar('\x14'));
	ASSERT_TRUE(IsXmlChar('\xFE'));
	ASSERT_FALSE(IsXmlChar('\xFF'));
}

TEST(CppStringEscape,ToOctal)
{
	ASSERT_EQ(ToOctal('\x0'),std::string("\\000"));
	ASSERT_EQ(ToOctal('\x7'),std::string("\\007"));
	ASSERT_EQ(ToOctal('\x8'),std::string("\\010"));
	ASSERT_EQ(ToOctal('\x3F'),std::string("\\077"));
	ASSERT_EQ(ToOctal('\x40'),std::string("\\100"));
}

TEST(CppStringEscape,CppStringEscape)
{
	ASSERT_EQ(CppStringEscape(std::string("The quck brown fox\fjumped over the lazy dog\n")),std::string("The quck brown fox\\014jumped over the lazy dog\x0A"));
}
