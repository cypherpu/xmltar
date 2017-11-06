/*
 * SnapshotFileEntryParser.hpp
 *
 *  Created on: Nov 6, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRYPARSER_HPP_
#define SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRYPARSER_HPP_

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
struct SnapshotFileEntryParser : qi::grammar<Iterator,SnapshotFileEntryParser()>
{
	SnapshotFileEntryParser() : SnapshotFileEntryParser::base_type(start,"SnapshotFileEntryParser")
	{
		using phoenix::construct;
		using phoenix::val;

		start =
				(
					   qi::eps[qi::_b=0.0]
			        >> ptime_day_slash_month_p(qi::_r1)
					>> qi::omit[+qi::char_(' ')]
					>> StockSymbolFromName_p
					>> -qi::omit
						[
						 	  +(qi::char_-qi::lit("You Bought")) >> qi::lit("You Bought")
							>> +qi::char_(' ')
						]
					>> thousands_separated_number_p												// number of shares
					>> qi::omit[+qi::char_(' ')]
					>> thousands_separated_number_p												// number of shares
					>> qi::omit[+qi::char_(' ')]
					>> qi::omit[thousands_separated_number_p[qi::_b=qi::_1] | qi::char_('-')]												// price
					>> qi::omit[+qi::char_(' ')]
					>> thousands_separated_number_p												// total cost including transaction fee
					>> qi::eol
					>> *qi::omit[+(qi::char_-qi::eol) >> qi::eol]
				)
				[phoenix::construct<SnapshotFileEntry>()
										(
												qi::_1,
												"BUY",
												phoenix::bind(&getAssetBySymbol,qi::_2),
												qi::_3,
												qi::_4,
												0.0,
												qi::_b,
												0.0,
												qi::_5
										)
							)]
				;

	using namespace qi;
    using namespace boost::phoenix;
    on_error<fail> ( start , std::cout << val("Error! Expecting ") << _4 << val(" here: \"") << construct<std::string>(_3, _2) << val("\"") << std::endl );
  }

  qi::rule<Iterator,SnapshotFileEntry()> start;
  StockSymbolFromName_parser<Iterator> StockSymbolFromName_p;
  ptime_day_slash_month_parser<Iterator> ptime_day_slash_month_p;
  thousands_separated_number_parser<Iterator> thousands_separated_number_p;
};

bool ParseSnapshotFileEntry(std::string input, unsigned int year, Fidelity::Individual::Account & account){
	  typedef std::string::const_iterator iterator_type;
	  std::string::const_iterator iter = input.begin();
	  std::string::const_iterator end = input.end();
	  Generic_Buy_2016_02_29_parser<iterator_type> Generic_Buy_2016_02_29_p;

	  bool r = parse(iter, end, Generic_Buy_2016_02_29_p(year), account );

	  if (iter==end) return true;

	  return false;
}

#endif /* SRC_COMMON_SNAPSHOT_SNAPSHOTFILEENTRYPARSER_HPP_ */
