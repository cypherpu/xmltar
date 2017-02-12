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

#include <boost/spirit/include/phoenix_operator.hpp> 		// for on_error
#include <boost/spirit/include/phoenix_object.hpp>			// for construct
#include <boost/spirit/include/phoenix_function.hpp>		// for construct
#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/home/phoenix/bind/bind_function.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>			// for bind

#include <boost/spirit/home/support/multi_pass.hpp>			// for multipass
#include <boost/spirit/home/support/multi_pass_wrapper.hpp>	// for multipass

#include "Incremental_File.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

void Incremental_File::Insert_Into_Map(const std::string filename, const time_t posix_time){
	std::string fn(XML_Attribute_Value_To_String(filename));

	if (incremental_data_map.find(fn)!=incremental_data_map.end())
		throw "Insert_Into_Map: filename duplicate in incremental file";

	Incremental_File::Incremental_Data id;
	id.last_backed_up_time=posix_time;
	incremental_data_map[fn]=id;
}

template <typename Iterator>
struct incremental_file_parser : qi::grammar<Iterator, Incremental_File(),  ascii::space_type>
{
	incremental_file_parser() : incremental_file_parser::base_type(start,"incremental_file")
    {
        using qi::int_;
        using qi::lit;
        using ascii::char_;

        text %= *(char_-'"')
				;

        start =
				lit("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>")
				> lit("<xmltar-incremental xmlns=\"http://www.xmltar.org/0.1\" version=\"") > text > lit("\">")
					> -(lit("<options>") > lit("</options>"))
					> *((lit("<file name=\"") > text > lit("\">")
						> lit("<last-backup-time posix=\"") > int_ > lit("\" localtime=\"") > text > lit("\"/>")
						> lit("</file>"))[phoenix::bind(&Incremental_File::Insert_Into_Map, qi::_val, qi::_1, qi::_2)])
				> lit("</xmltar-incremental>")
				;

        start.name("start");
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;
        on_error<fail> ( start , std::cout << val("Error! Expecting ") << _4 << val(" here: \"") << construct<std::string>(_3, _2) << val("\"") << std::endl );
    }

    qi::rule<Iterator, std::string(), ascii::space_type> text;
    qi::rule<Iterator, Incremental_File(), ascii::space_type> start;
};

void Incremental_File::Read_Incremental_File(std::ifstream& ifs, bool compress){
	  typedef std::istreambuf_iterator<char> base_iterator_type;
	  boost::spirit::multi_pass<base_iterator_type> first =
	      boost::spirit::make_default_multi_pass(base_iterator_type(ifs.rdbuf()));
	  boost::spirit::multi_pass<base_iterator_type> last =
	      boost::spirit::make_default_multi_pass(base_iterator_type());

	  incremental_file_parser<boost::spirit::multi_pass<base_iterator_type> > incremental_file_parser_instance;

	  bool r = phrase_parse(first,
							last,
							incremental_file_parser_instance,
							boost::spirit::ascii::space,
							*this);
}
