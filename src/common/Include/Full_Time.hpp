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

#ifndef Full_Time_hpp
#define Full_Time_hpp

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_operator.hpp> 		// for on_error
#include <boost/spirit/include/phoenix_object.hpp>			// for construct
#include <boost/spirit/include/phoenix_function.hpp>		// for construct

#include "Xml_Attribute.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix=boost::phoenix;

struct full_time {
	time_t posix_time_;
	std::string formatted_time_;
};
#if 0
BOOST_FUSION_ADAPT_STRUCT(
		full_time,
		(time_t, posix_time_)
		(std::string, formatted_time_)
)
#endif
template <typename Iterator>
struct full_time_parser : qi::grammar<Iterator, full_time(),  ascii::space_type>
{
	full_time_parser() : full_time_parser::base_type(start,"full_time")
    {
        using qi::int_;
        using qi::lit;
        using ascii::char_;
        using boost::spirit::lexeme;

        start %=
        	lit("posix=\"") > lexeme[int_ > char_('"')]
        	> lit("localtime") > xml_attribute_p
            ;

        start.name("start");

        using namespace qi;
        using namespace boost::phoenix;
        on_error<fail> ( start , std::cout << val("Error! Expecting ") << _4 << val(" here: \"") << construct<std::string>(_3, _2) << val("\"") << std::endl );
    }

	xml_attribute_parser<Iterator> xml_attribute_p;
    qi::rule<Iterator, full_time(), ascii::space_type> start;
};

#endif /* Full_Time_hpp */
