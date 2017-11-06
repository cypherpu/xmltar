/*
 * SnapshotOptionsParser.hpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTOPTIONSPARSER_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTOPTIONSPARSER_HPP_

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/variant/recursive_variant.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix=boost::phoenix;

template <typename Iterator>
struct SnapshotOptionsParser : qi::grammar<Iterator,SnapshotOptions()>
{
	SnapshotOptionsParser() : SnapshotOptionsParser::base_type(start,"SnapshotOptionsParser")
	{
		using phoenix::construct;
		using phoenix::val;

		start =
				(
					   lit("<options>")
			        >> lit("</options")
				)
				[phoenix::construct<SnapshotOptions>()]
				;

	using namespace qi;
    using namespace boost::phoenix;
    on_error<fail> ( start , std::cout << val("Error! Expecting ") << _4 << val(" here: \"") << construct<std::string>(_3, _2) << val("\"") << std::endl );
  }

  qi::rule<Iterator,SnapshotOptions()> start;
};

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTOPTIONSPARSER_HPP_ */
