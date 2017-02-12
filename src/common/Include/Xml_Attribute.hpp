/*

Copyright 2010 by David A. Betz

This file is part of xmltar.

xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xmltar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef Xml_Attribute_hpp_
#define Xml_Attribute_hpp_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp> 		// for on_error
#include <boost/spirit/include/phoenix_object.hpp>			// for construct
#include <boost/spirit/include/phoenix_function.hpp>		// for construct

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix=boost::phoenix;

template <typename Iterator>
struct xml_attribute_parser : qi::grammar<Iterator, std::string(),  ascii::space_type>
{
	xml_attribute_parser() : xml_attribute_parser::base_type(start,"xml_attribute")
    {
        using qi::int_;
        using qi::lit;
        using ascii::char_;
        using boost::spirit::lexeme;

        start %=
        	lit("=")
        	> lit("\"")
        	>lexeme[*(char_ -'"')]
        	> lit("\"")
			;

        start.name("start");

        using namespace qi;
        using namespace boost::phoenix;
        on_error<fail> ( start , std::cout << val("Error! Expecting ") << _4 << val(" here: \"") << construct<std::string>(_3, _2) << val("\"") << std::endl );
    }

    qi::rule<Iterator, std::string(), ascii::space_type> start;
};


#endif /* Xml_Attribute_hpp_ */
