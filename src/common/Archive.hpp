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

#ifndef Archive_hpp_
#define Archive_hpp_

#include <sstream>

#include <boost/scoped_ptr.hpp>

#include "Debug.hpp"
#include "Xmltar/XmltarInvocation.hpp"

class Archive_Member;

class Archive {
        XmltarOptions options_;
        int archive_sequence_number;
		std::string filename;
		std::streamoff last_flush_position;
        std::string archive_header;
		std::string archive_trailer;
		bool is_attached;
		size_t volumes_written;

		std::fstream fs;
		std::ofstream ofs;
		std::ostream* os;

		std::string Generate_Archive_Header(void);
		std::string Generate_Archive_Trailer_Plaintext(bool last_volume, int padding);
		std::string Encode_String(std::string s);
		std::string Generate_Archive_Trailer(bool last_volume, int padding);
		bool Is_Trailer(std::string s);
		void Initialize(void);
	public:
		Archive(const XmltarOptions&, int);
		~Archive(void){
		}
		size_t Store_Member(Archive_Member& am);

		void Attach(void);
		bool Is_Attached(void){ return is_attached; }
		bool Has_Space_For_N_Bytes_Of_Member(Archive_Member& am, size_t n, bool include_member_header);
		size_t Find_Maximum_Writable_Size(Archive_Member *am, bool include_member_header);
		bool Has_Space_For_Part_Of_Member(Archive_Member* am, bool include_member_header);
		void Store_Part_Of_Member(Archive_Member *am);
		size_t Size(void);
		void Close(bool last_volume);
		void Close_Transitbuf(void);

		void Write_Archive_Header(void);

		std::string Archive_Header(void){ return archive_header; }
		std::string Archive_Trailer(void){ return archive_trailer; }
};

#endif /* Archive_hpp_ */
