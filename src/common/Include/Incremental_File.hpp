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

#ifndef Incremental_File_hpp_
#define Incremental_File_hpp_

#if 0

#include <string>
#include <map>
#include <fstream>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>

#include "Archive_Member.hpp"

class Incremental_File {
	public:
		class Incremental_Data {
		public:
			time_t last_backed_up_time;
		};
	protected:
		std::map<std::string,Incremental_Data> incremental_data_map;
		std::string filename_;
		bool is_compressed;
		boost::scoped_array<char> temp_filename_;
		std::fstream incremental_os;
		time_t backup_time;
	public:
		Incremental_File(void){ }

		void Open(std::string filename, bool compress){
			filename_=filename;
			is_compressed=compress;

			boost::filesystem::path f=filename;
			boost::filesystem::file_status s=boost::filesystem::symlink_status(f);

			if (boost::filesystem::exists(s)){
				if (!boost::filesystem::is_regular(s))
					throw "Incremental_File::Incremental_File: incremental file is not a regular file";

				std::ifstream ifs;
				ifs.open(filename.c_str(),std::ios_base::in | std::ios_base::out);
				ifs.seekg(0,std::ios_base::beg);
				char buf[1024];
				ifs.read(buf,2);

				bool has_gzip_wrapper;
				if (buf[0]=='\x1f' && buf[1]=='\x8b')
					has_gzip_wrapper=true;
				else if (buf[0]=='<' && buf[1]=='?')
					has_gzip_wrapper=false;
				else throw "Incremental_File::Incremental_File: unrecognized file format "+f.string();

				if (compress && !has_gzip_wrapper)
					throw "Incremental_File::Incremental_File: compression requested, but incremental file is not compressed";
				if (!compress && has_gzip_wrapper)
					throw "Incremental_File::Incremental_File: compression not requested, but incremental file is compressed";

				ifs.seekg(0,std::ios_base::beg);
				Read_Incremental_File(ifs, compress);
				ifs.close();
			}

			filename+="XXXXXX";
			filename+='\0';
			temp_filename_.reset(new char[filename.size()]);
			for(unsigned int i=0; i<filename.size(); ++i)
				temp_filename_[i]=filename[i];

			int fd=mkstemp(temp_filename_.get());
			incremental_os.open(temp_filename_.get());
			close(fd);
			backup_time=time(0);
		}
		~Incremental_File(void){
		}
		void Read_Incremental_File(std::ifstream&, bool);
		void Write(){
			incremental_os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;
			incremental_os << "<xmltar-incremental xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">" << std::endl;

			for(std::map<std::string,Incremental_Data>::iterator i=incremental_data_map.begin(); i!=incremental_data_map.end(); ++i)
				incremental_os	<< "\t<file name=\"" << String_To_XML_Attribute_Value(i->first) << "\">\n"
									<< "\t\t<last-backup-time posix=\"" << i->second.last_backed_up_time << "\" "
									<< "localtime=\"" << To_Local_Time(i->second.last_backed_up_time) << "\"/>\n"
								<< "\t</file>\n";

			incremental_os << "</xmltar-incremental>\n";
		}
		void Update(Archive_Member& am){
			incremental_data_map[am.File_Path().string()].last_backed_up_time=backup_time;
		}
		void Close(void){
			incremental_os.close();
			boost::filesystem::remove(filename_.c_str());
			boost::filesystem::rename(boost::filesystem::path(temp_filename_.get()),boost::filesystem::path(filename_));
		}
		bool Is_Current(Archive_Member& am){
			if (incremental_data_map.find(am.File_Path().string())==incremental_data_map.end())
				return false;
			else
				return incremental_data_map[am.File_Path().string()].last_backed_up_time>am.Last_Write_Time();
		}
		void Insert_Into_Map(std::string, time_t);
};

#endif

#endif /* Incremental_File_hpp_ */
