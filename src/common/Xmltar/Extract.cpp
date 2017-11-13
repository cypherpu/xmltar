/*

Copyright 2010 by David A. Betz

This file is part of xmltar.

xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 *
xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xmltar.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <sstream>
#include <cctype>
#include <boost/format.hpp>
#include <sys/stat.h>
#include "Conversions.hpp"
#include <boost/spirit/home/support/multi_pass.hpp>
#include <boost/spirit/home/support/multi_pass_wrapper.hpp>
#include <boost/spirit/repository/include/qi_flush_multi_pass.hpp>
#include <boost/spirit/include/qi.hpp>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <complex>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>

//#include <boost/spirit/home/phoenix/bind/bind_function.hpp>
//#include <boost/spirit/home/phoenix/function/function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
//#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>

#include <boost/spirit/include/qi_grammar.hpp>

#include <boost/spirit/include/qi.hpp>
#include "Xmltar/Xmltar.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix=boost::phoenix;

#include "Meta_Data_Ext4.hpp"

struct multivolume_data {
    std::string multivolume_name_pattern_;
    int multivolume_sequence_number_;
};

BOOST_FUSION_ADAPT_STRUCT(
        multivolume_data,
        (std::string, multivolume_name_pattern_)
        (int, multivolume_sequence_number_)
)

int foo_bar_bletch;

struct archive_extractor {
    std::string archive_name_;
    boost::optional<multivolume_data> mv_data_;
    std::string filename_;
    boost::optional<std::string> tag_;
    std::string filetype_;
    boost::optional<meta_data_ext4> metadata_ext4_;
    std::string precompress_;
    std::string encode_;
    int file_size_;
    int this_extent_start_;
    std::string symlink_target;
    boost::scoped_ptr<transitbuf_base> encoder_;
    boost::scoped_ptr<transitbuf_base> precompressor_;
    std::fstream ofs_;
    boost::optional<std::string> last_volume_;
    std::vector<boost::filesystem::path> source_files;
public:
    archive_extractor(std::vector<boost::filesystem::path>& s)
        : source_files(s) { }
    void Archive_Name(std::string name) { archive_name_=name; }
    void Multivolume_Name_Pattern_and_Sequence_Number(boost::optional<multivolume_data>& mv_data){ mv_data_=mv_data; }
    void Filename(std::string filename, boost::optional<std::string> tag){ filename_=filename; tag_=tag; std::cerr << "filename=" << filename_ << std::endl; }
    void Metadata(boost::optional<meta_data_ext4>& metadata){ metadata_ext4_=metadata; }
    void Content_Type(std::string filetype){ filetype_=filetype; }
    void Directory(void){ }
    void Regular_File(std::string precompress, std::string encode, int file_size, int this_extent_start){
        precompress_=precompress;
        encode_=encode;
        file_size_=file_size;
        this_extent_start_=this_extent_start;

        if (encoder_.get()==0 && precompressor_.get()==0){
            if (encode_=="base16") encoder_.reset(new base16_decoder_transitbuf);
            else if (encode_=="base64") encoder_.reset(new base64_decoder_transitbuf);

            if (precompress_=="identity") precompressor_.reset(new identity_decoder_transitbuf);
            else if (precompress=="gzip") precompressor_.reset(new gzip_decoder_transitbuf);
            else if (precompress=="bzip2") precompressor_.reset(new bzip2_decoder_transitbuf);
            else if (precompress=="xz") precompressor_.reset(new xz_decoder_transitbuf);
            else throw "archive_extractor::Process_Char: unrecognized precompress";

            boost::filesystem::path f=filename_;
            boost::filesystem::file_status s=boost::filesystem::symlink_status(f);
            if (!boost::filesystem::exists(s))
                ofs_.open(filename_.c_str(), std::ios_base::out | std::ios_base::binary);
            else {
                ofs_.open(filename_.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary);
                ofs_.seekp(0,std::ios_base::beg);
                std::streampos ofs_beg=ofs_.tellp();
                if (ofs_beg==std::streampos(-1)){
                    std::cerr << "cannot ofs_beg" << std::endl;
                    throw;
                }
                ofs_.seekp(0,std::ios_base::end);
                std::streampos ofs_end=ofs_.tellp();
                if (ofs_end==std::streampos(-1)){
                    std::cerr << "cannot ofs_end" << std::endl;
                    throw;
                }
                if (ofs_end-ofs_beg<this_extent_start_)
                    for(size_t i=ofs_end-ofs_beg; i<this_extent_start_; ++i)
                        ofs_.put(0);
            }

            precompressor_->open(std::ios_base::out,ofs_.rdbuf());
            encoder_->open(std::ios_base::out,precompressor_.get());
        }
    }
    void Symlink(std::string target){ if (symlink(target.c_str(),filename_.c_str())!=0) throw "archive_extractor::Symlink: unable to create symlink"; }
    void Process_Char(char c){
        if (encoder_->sputc(c)!=c) throw "archive_extractor::Process_Char: unable to sputc";
    }
    void End_File(void){
        if (filetype_=="directory")
            if (mkdir(filename_.c_str(),0700)!=0) throw "archive_extractor:End_File: unable to mkdir";
            else ;
        else if (filetype_=="regular"){
            encoder_->close();
            precompressor_->close();
            ofs_.close();

            precompressor_.reset(0);
            encoder_.reset(0);
        }
    }
    void Last_Volume(boost::optional<std::string> lv){
        last_volume_=lv;
    }
};

template <typename Iterator>
struct archive_extractor_grammar : qi::grammar<Iterator, void(archive_extractor*), qi::locals<std::string>, ascii::space_type>
{
    archive_extractor_grammar() : archive_extractor_grammar::base_type(start){
        using qi::int_;
        using qi::lit;
        using ascii::char_;
        using boost::spirit::lexeme;

        content %= *(char_ - '<')
            ;

        xml_attribute_p %=
            lit("=")
            > lit("\"")
            >lexeme[*(char_ -'"')]
            > lit("\"")
            ;

        extended_attribute_p %=
            lit("<extended-attribute key") > xml_attribute_p > lit("value") > xml_attribute_p > lit("/>")
            ;

        full_time_p %=
            lit("posix=\"") > lexeme[int_ > char_('"')]
            > lit("localtime") > xml_attribute_p
            ;

        metadata_ext4_p %=
            lit("<meta-data>")
                > *extended_attribute_p
                > lit("<mode value=\"") > qi::oct > lit("\"/>")
                > lit("<atime") > full_time_p > lit("/>")
                > lit("<ctime") > full_time_p > lit("/>")
                > lit("<mtime") > full_time_p > lit("/>")
                > lit("<user uid=\"") > int_ > lit("\"") > lit("uname") > xml_attribute_p > lit("/>")
                > lit("<group gid=\"") > int_ > lit("\"") > lit("gname") > xml_attribute_p > lit("/>")
                > lit("<size value=\"") > int_ > lit("\"/>")
            > lit("</meta-data>")
            ;

        multivolume_data_p %=
            lit("<multi-volume>")
                > lit("<name-pattern>") > content > lit("</name-pattern>")
                > lit("<sequence-number>") > int_ > lit("</sequence-number>")
            > lit("</multi-volume>")
            ;

        start =
            lit("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>")
            > lit("<xmltar xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">")
                > lit("<options>")
                > lit("<archive-name>") > content [phoenix::bind(&archive_extractor::Archive_Name,qi::_r1,qi::_1)] > lit("</archive-name>")
                > (-multivolume_data_p) [phoenix::bind(&archive_extractor::Multivolume_Name_Pattern_and_Sequence_Number,qi::_r1,qi::_1)]
                > lit("</options>")
                > lit("<members>")






                    > *((lit("<file name") > xml_attribute_p > -(lit("tag") > xml_attribute_p) > lit(">")) [phoenix::bind(&archive_extractor::Filename,qi::_r1,qi::_1,qi::_2)]
                        > (-(metadata_ext4_p)) [phoenix::bind(&archive_extractor::Metadata,qi::_r1,qi::_1)]
                        > (
                           ((lit("<content type=\"") >> qi::string("regular") >> lit("\">")) [phoenix::bind(&archive_extractor::Content_Type,qi::_r1,qi::_1)]
                              > ((lit("<stream name=\"data\"")
                                        > lit("pre-compression") > xml_attribute_p
                                        > lit("encoding") > xml_attribute_p
                                        > lexeme[lit("total-size=\"") > int_ > lit("\"")]
                                        > lexeme[lit("this-extent-start=\"") > int_ > lit("\">")])[phoenix::bind(&archive_extractor::Regular_File,qi::_r1,qi::_1,qi::_2,qi::_3,qi::_4)]


                                        > *(char_("0-9a-zA-Z+=/")[phoenix::bind(&archive_extractor::Process_Char,qi::_r1,qi::_1)])
                                    >> lit("</stream>"))
                            > lit("</content>"))
                           |
                        ((lit("<content type=\"") >> qi::string("symlink") >> lit("\">")) [phoenix::bind(&archive_extractor::Content_Type,qi::_r1,qi::_1)]
                            > (lit("<symlink target") > xml_attribute_p > lit("/>")) [phoenix::bind(&archive_extractor::Symlink,qi::_r1,qi::_1)]
                         > lit("</content>"))
                        |
                        ((lit("<content type=\"") >> qi::string("directory") >> lit("\"/>")) [phoenix::bind(&archive_extractor::Content_Type,qi::_r1,qi::_1)])
                       )
                    > lit("</file>")[phoenix::bind(&archive_extractor::End_File,qi::_r1)])






                > lit("</members>")
                > (-(lit("<is-last-volume value") > xml_attribute_p > lit("padding") > qi::omit[xml_attribute_p] > "/>")) [phoenix::bind(&archive_extractor::Last_Volume,qi::_r1,qi::_1)]
            > lit("</xmltar>")
            ;

        start.name("start");

        using namespace qi;
        using namespace boost::phoenix;
        on_error<fail> ( start , std::cout << val("Error! Expecting ") << _4 << val(" here: \"") << construct<std::string>(_3, _2) << val("\"") << std::endl );
    }

    qi::rule<Iterator, void(archive_extractor*), qi::locals<std::string>, ascii::space_type> start;
    qi::rule<Iterator, std::string(), ascii::space_type> content;
    qi::rule<Iterator, std::string(), ascii::space_type> xml_attribute_p;
    qi::rule<Iterator, meta_data_ext4(), ascii::space_type> metadata_ext4_p;
    qi::rule<Iterator, full_time(), ascii::space_type> full_time_p;
    qi::rule<Iterator, extended_attribute(), ascii::space_type> extended_attribute_p;
    // qi::rule<Iterator, void(archive_extractor*), ascii::space_type> content_p;
    qi::rule<Iterator, multivolume_data(), ascii::space_type> multivolume_data_p;
};

void Xmltar::Extract(void){
	std::cerr << "Xmltar::Extract: entering" << std::endl;

	for(options_.current_sequence_number=options_.starting_sequence_number; ; ++options_.current_sequence_number){
		std::istream *is;
		std::ifstream ifs;

		std::string filename;

		if (options_.multi_volume){
			boost::format fmt(options_.base_xmltar_file_name);
			fmt % options_.current_sequence_number;
			filename=str(fmt);
		}
		else
			filename=options_.base_xmltar_file_name;

		std::cerr << "filename=" << filename << std::endl;

		if (filename=="-"){
			is=&std::cin;
		}
		else{
				ifs.open(filename.c_str());
				if (!ifs.is_open())
					throw "Xmltar::Extract: could not open input file";

				is=&ifs;
		}

        boost::scoped_ptr<transitbuf_base> post(options_.New_Postdecompressor());

		post->open(std::ios_base::in,is->rdbuf());
/*
		typedef std::istreambuf_iterator<char> base_iterator_type;
		boost::spirit::multi_pass<base_iterator_type> first =
			boost::spirit::make_default_multi_pass(base_iterator_type(post.get()));
		boost::spirit::multi_pass<base_iterator_type> last =
			boost::spirit::make_default_multi_pass(base_iterator_type());
        archive_extractor_grammar<boost::spirit::multi_pass<base_iterator_type> > archive_extractor_grammar_instance;
*/
#if 0
        std::ostringstream oss;
        oss << post.get();
        std::string input(oss.str());
#endif

        std::string input, line;
        while(getline(*is,line)){
        	input+=line;
        	std::cerr << line << std::endl;
        }
	    std::string::const_iterator first = input.begin();
	    std::string::const_iterator last = input.end();
        archive_extractor_grammar<std::string::const_iterator > archive_extractor_grammar_instance;

		archive_extractor archive_extractor_instance(options_.source_files);

        if (!phrase_parse(  first,
                            last,
                            archive_extractor_grammar_instance(&archive_extractor_instance),
                            boost::spirit::ascii::space)){
            for(int i=0; i<100; ++i, ++first)
                std::cerr << *first;
            throw "unable to parse header";
        }
        else std::cerr << "parsed header" << std::endl;

        if (is==&ifs){
            is=0;
            ifs.close();
        }

        if (archive_extractor_instance.last_volume_)
            if (*(archive_extractor_instance.last_volume_)=="false") continue;

        break;
	}
}
