/*
 * XmlEscapeAttribute.Test.cpp
 *
 *  Created on: Jul 25, 2018
 *      Author: dbetz
 */

#include <gmock/gmock.h>

#include "Utilities/XmlEscapeAttribute.hpp"

TEST(XmlEscapeAttribute,XmlEscapeAttribute)
{
	ASSERT_EQ(XmlEscapeAttribute("&\"'<>"),std::string("&amp;&quot;&apos;&lt;&gt;"));
}

TEST(XmlEscapeAttribute,XmlUnEscapeAttribute)
{
	ASSERT_EQ(std::string("&amp;&quot;&apos;&lt;&gt;"),XmlEscapeAttribute("&\"'<>"));
}
