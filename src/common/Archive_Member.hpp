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

#ifndef Archive_Member_hpp_
#define Archive_Member_hpp_

#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <pwd.h>
#include <grp.h>
}

#include "Debug.hpp"
#include "Xmltar/Xmltar.hpp"
#include "Conversions.hpp"
#include "Pipe_Codecs.hpp"

class Archive_Member {
		XmltarOptions options_;
		boost::filesystem::path file_path;
		boost::filesystem::file_status f_stat;
		boost::filesystem::file_type f_type;
		struct stat stat_buf;
		off_t file_size;
		off_t start_tell;
		bool metadata_written;
		boost::scoped_ptr<transitbuf_base> encoder;
		boost::scoped_ptr<transitbuf_base> precompressor;
		transitbuf_base *archive_tb;
		std::ifstream ifs;
		std::string archive_member_header, archive_member_trailer;
		bool is_attached;
		bool is_empty;

		std::string Generate_Metadata();
		std::string Generate_Archive_Member_Header();
		std::string Generate_Archive_Member_Trailer(void);

    public:
        /*
         * when we open an Archive_Member, we may be in a situation where the existing archive does
         * not have enough room to accommodate even the smallest member. If we prematurely connect
         * the transit_buf to the output of the encoders, we may (in theory) write bytes to the end
         * of our archive, without being able to write the entire member.
         *
         * to get around this problem, we will use a temporary ostringstream destination for the
         * results of our encoder. When we know whether we'll actually be able to add to the end
         * of the archive, we will transfer any contents in the ostringstream to the archive, and
         * then replace the ostringstream destination with a transitbuf destination
         */
		Archive_Member(boost::filesystem::path& fpath, const XmltarOptions& opt);

		~Archive_Member(void){
			DEBUGCXX(debugcxx,"Archive_Member::~Archive_Member");
			ifs.close();
		}

        void Reset_Encoder_and_Precompressor(void){
            encoder.reset(options_.New_Encoder());
            precompressor.reset(options_.New_Precompressor());
        }

		void Attach(transitbuf_base *tb);

		bool Is_Attached(void){ return is_attached; }

		bool Is_Empty(void){
			DEBUGCXX(debugcxx,"Archive_Member::Is_Empty");

			return is_empty;
		}

		boost::filesystem::path File_Path(void){ return file_path; }

		bool Is_Regular(void){ return boost::filesystem::is_regular(f_stat); }

		size_t File_Size(void){ return file_size; }

		time_t Last_Write_Time(void){ return boost::filesystem::last_write_time(file_path); }

		void Write(size_t n);

		size_t Delta_Encoded_Length(size_t n, bool include_header);

		void Close_Encoders(void){
		    if (Is_Attached()){
		        precompressor->close();
		        encoder->close();
		    }

			precompressor.reset(0);
			encoder.reset(0);

			is_attached=false;
		}

		void Reset_Destination_Stream(transitbuf_base *tb){
			archive_tb=tb;
			encoder->rdbuf(archive_tb);
			is_attached=true;
		}

		void Write_Archive_Member_Header(void){
			archive_tb->sputn(archive_member_header.c_str(), archive_member_header.size());
			archive_tb->pubsync();
			metadata_written=true;
			archive_member_header=Generate_Archive_Member_Header();

            if (!Is_Regular()) is_empty=true;
		}

		void Write_Archive_Member_Trailer(void){
			archive_tb->sputn(archive_member_trailer.c_str(), archive_member_trailer.size());
			archive_tb->pubsync();
		}

		std::string Archive_Member_Header(void){ return archive_member_header; }

		std::string Archive_Member_Trailer(void){ return archive_member_trailer; }
};

#endif /*Archive_Member_hpp_*/
