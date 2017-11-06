/*
 * SnapshotParser.hpp
 *
 *  Created on: Nov 5, 2017
 *      Author: dbetz
 */

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

#include "Snapshot/Snapshot.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix=boost::phoenix;

template <typename Iterator>
struct SnapshotParser : qi::grammar<Iterator,Snapshot()>
{
	SnapshotParser() : SnapshotParser::base_type(start,"SnapshotParser")
	{
		using phoenix::construct;
		using phoenix::val;

		start =	(
					   qi::lit("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
					>> qi::lit("<xmltar-snapshot xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">")
					>> *qi::lit('\t')
					>> qi::lit("<options>")
					>> *qi::lit('\t')
					>> qi::lit("</options>")
					>> *qi::lit('\t')
					>> qi::lit("<file-sequence>")
				)[phoenix::construct<Snapshot>()]
#if 0
					>> qi::lit("</options>")
					>> qi::lit("<file-sequence>")
						>> *(
								   qi::lit("<file>")
									>> qi::lit("<path>")
										>> qi::string
									>> qi::lit("</path>")
									>> qi::lit("<last-backup-epoch-time>")
										>> qi::ulong_
									>> qi::lit("</last-backup-epoch-time>")
									>> qi::lit("<media-number-sequence>")
										>> (*(qi::uint_)[phoenix::push_back(phoenix::ref(qi::_val),qi::_1)])
									>> qi::lit("</media-number-sequence>")
								>> qi::lit("</file>")
							)
					>> qi::lit("</file-sequence>")
					>> qi::lit("</xmltar-snapshot>")
#endif
				;

	using namespace qi;
    using namespace boost::phoenix;
    on_error<fail> ( start , std::cout << val("Error! Expecting ") << _4 << val(" here: \"") << construct<std::string>(_3, _2) << val("\"") << std::endl );
  }

  qi::rule<Iterator,Snapshot()> start;
};

bool ParseSnapshot(std::string input, Snapshot & snapshot){
	  typedef std::string::const_iterator iterator_type;
	  std::string::const_iterator iter = input.begin();
	  std::string::const_iterator end = input.end();
	  SnapshotParser<iterator_type> Snapshot_p;

	  bool r = parse(iter, end, Snapshot_p, snapshot );

      std::string::const_iterator some = iter+400;
      std::string context(iter, (some>end)?end:some);
      std::cerr << "stopped at:\n\"" << context << "...\"\n";

	  if (iter==end) return true;

	  return false;
}
