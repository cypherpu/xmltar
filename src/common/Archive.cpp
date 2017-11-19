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

#include "Archive.hpp"

#include <boost/format.hpp>
#include <boost/random.hpp>
#include <boost/spirit/include/qi.hpp>

#include "Archive_Member.hpp"
#include "Conversions.hpp"

std::string Archive::Generate_Archive_Header(void){
    std::string s;

    s+="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"+options_.Newline();
    s+="<xmltar xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">"+options_.Newline();
    s+=options_.Tabs("\t")+"<options>"+options_.Newline();
    s+=options_.Tabs("\t\t")+"<archive-name>"+filename+"</archive-name>"+options_.Newline();
    if (options_.multi_volume_){
        s+=options_.Tabs("\t\t")+"<multi-volume>"+options_.Newline();
        s+=options_.Tabs("\t\t\t")+"<name-pattern>"+options_.base_xmltar_file_name_.get()+"</name-pattern>"+options_.Newline();
        s+=options_.Tabs("\t\t\t")+"<sequence-number>"+To_Decimal_Int(archive_sequence_number)+"</sequence-number>"+options_.Newline();
        s+=options_.Tabs("\t\t")+"</multi-volume>"+options_.Newline();
    }
    s+=options_.Tabs("\t")+"</options>"+options_.Newline();
    s+=options_.Tabs("\t")+"<members>"+options_.Newline();

    return s;
}

std::string Archive::Generate_Archive_Trailer_Plaintext(bool last_volume, int padding){
    boost::mt19937 rng;
    boost::uniform_int<> dist(97,122);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
        die(rng, dist);

    std::string plaintext_trailer=options_.Tabs("\t")+"</members>"+options_.Newline();
    if (options_.multi_volume_)
        if (last_volume)
			plaintext_trailer+=options_.Tabs("\t")+"<is-last-volume value=\"true\" padding=\""+std::string(0,' ')+"\"/>"+options_.Newline();
        else {
            plaintext_trailer+=options_.Tabs("\t")+"<is-last-volume value=\"false\" padding=\"";
            for(int i=0; i<padding; ++i)
                plaintext_trailer+=(char) die();
            plaintext_trailer+="\"/>"+options_.Newline();
        }
    else plaintext_trailer+=options_.Tabs("\t")+"<is-last-volume value=\"true\" padding=\""+std::string(0,' ')+"\"/>"+options_.Newline();

    plaintext_trailer+="</xmltar>"+options_.Newline();

    return plaintext_trailer;
}

std::string Archive::Encode_String(std::string s){
    std::stringstream tmp(std::ios_base::out);

    return tmp.str();
}

std::string Archive::Generate_Archive_Trailer(bool last_volume, int target_size){       // TODO: Encode_String is called twice for each Generate_Archive_Trailer
    DEBUGCXX(debugcxx,"Archive::Archive_Trailer");
    int padding;
    std::string result;
    std::string null_result=Encode_String(Generate_Archive_Trailer_Plaintext(last_volume, 0));

    if (target_size!=0) padding=target_size-null_result.size();
    else padding=0;

    for( int guess_padding=padding, count=0; count<10; ++count){
        result=Encode_String(Generate_Archive_Trailer_Plaintext(last_volume, guess_padding));

        if (padding==0) break;
        if (!options_.multi_volume_) break;
        if (options_.multi_volume_ && last_volume) break;
        if (result.size()>target_size) --guess_padding;
        else if (result.size()==target_size) break;
        else if (result.size()<target_size) guess_padding+=target_size-result.size();
    }

    return result;
}

bool Archive::Is_Trailer(std::string s){
}

void Archive::Initialize(void){
        filename="";
        last_flush_position=0;
        archive_header="";
        archive_trailer="";
        is_attached=false;
        os=0;

        if (fs.is_open()) fs.close();
        if (ofs.is_open()) ofs.close();

        if (options_.multi_volume_){
            boost::format fmt(options_.base_xmltar_file_name_.get());
            fmt % archive_sequence_number;
            filename=str(fmt);
        }
        else
            filename=options_.base_xmltar_file_name_.get();

        if (filename=="-"){
            if (options_.multi_volume_)
                throw "Archive::Initialize: archive file cannot be stdout with multivolume option";
            os=&std::cout;
        }
        else
            if (options_.operation_==XmltarOptions::CREATE){
                ofs.open(filename.c_str());
                os=&ofs;
                archive_header=Generate_Archive_Header();
            }
            else if (options_.operation_==XmltarOptions::APPEND){
                boost::filesystem::path f=filename;
                boost::filesystem::file_status s=boost::filesystem::symlink_status(f);
                if (!boost::filesystem::exists(s))
                    throw "Archive::Initialize: APPEND target does not exist";
                if (!boost::filesystem::is_regular(s))
                    throw "Archive::Initialize: xmltar file is not a regular file";
                if (file_size(f)<4)
                    throw "Archive::Initialize: xmltar file has a size<6";

                fs.open(filename.c_str(),std::ios_base::in | std::ios_base::out | std::ios_base::ate);
                fs.seekg(0,std::ios_base::beg);
                std::streampos origin=fs.tellg();
                char buf[1024];
                fs.read(buf,6);
#if 0
                std::string headerMagic=options_.HeaderMagicNumber();
                switch(options_.postcompress){
					case Compression::IDENTITY:
					case Compression::GZIP:
					case Compression::BZIP2:
					case Compression::LZIP:
						if (std::string(buf,headerMagic.size())!=headerMagic)
							throw "Archive::Initialize: cannot mix different types of compression in old and new archives";
						break;
					default:
						throw "Archive::Initialize: unrecognized compression";
                }
#endif
                off_t this_file_size=boost::filesystem::file_size(f);
                size_t i;
                //std::string trailerMagic=TrailerMagicNumber();
                for(i=10; ; ++i){
                    if (i>=sizeof(buf) || i>=this_file_size)
                        throw "Archive::Initialize: very long archive trailer";
                    fs.seekg(-i,std::ios_base::end);
                    fs.read(buf,10);
                    //if (std::string(buf,trailerMagic.size())!=trailerMagic) continue;

                    fs.seekg(-i,std::ios_base::end);
                    fs.read(buf,std::min(i,sizeof(buf)));
                    if (Is_Trailer(std::string(buf,fs.gcount()))){
                        fs.seekg(-i,std::ios_base::end);
                        fs.seekp(-i,std::ios_base::end);
                        break;
                    }
                }

                std::streampos curpos=fs.tellg();
                last_flush_position=curpos-origin;
                os=&fs;
                archive_header="";
            }
            else throw "Archive::Initialize: operation must be CREATE or APPEND";


        archive_trailer=Generate_Archive_Trailer(false,0);
}

size_t Archive::Store_Member(Archive_Member& member){
    if (!options_.multi_volume_){
        Store_Part_Of_Member(&member);

        if (!member.Is_Empty())
            throw "Xmltar::Add_To_Archive: cannot archive member";
    }
    else {
        if (Has_Space_For_Part_Of_Member(&member,true)){
            if (!Is_Attached()) Attach();
            Store_Part_Of_Member(&member);
        }


        while(!member.Is_Empty()){
            Close(false);
            ++volumes_written;

            ++archive_sequence_number;
            options_.operation_=XmltarOptions::CREATE;
            Initialize();
            if (!Has_Space_For_Part_Of_Member(&member,true))
                throw "Xmltar::Add_To_Archive: no room for member in empty archive";

            Attach();
            Write_Archive_Header();
            Store_Part_Of_Member(&member);
        }
    }

    return volumes_written;
}

Archive::Archive(const XmltarOptions& options, int sequence_number)
    : options_(options), archive_sequence_number(sequence_number), volumes_written(0)
{
    DEBUGCXX(debugcxx,"Archive::Archive");

    Initialize();
}

void Archive::Attach()
{
    is_attached=true;
}

bool Archive::Has_Space_For_N_Bytes_Of_Member(Archive_Member& am, size_t n, bool include_member_header){
    DEBUGCXX(debugcxx,"Archive::Has_Space_For_N_Bytes_Of_Member");

    bool result;

    return result;
}

size_t Archive::Find_Maximum_Writable_Size(Archive_Member *am, bool include_member_header){
    DEBUGCXX(debugcxx,"Archive::Find_Maximum_Writable_Size");
    unsigned int lb(0), ub(am->File_Size());

    for( ; ; ){
        int mid(lb+(ub-lb)/2);

        if (!Has_Space_For_N_Bytes_Of_Member(*am, mid, include_member_header)) ub=mid;
        else lb=mid;

        if (ub-lb==0) break;
        else if (ub-lb==1){
            if (Has_Space_For_N_Bytes_Of_Member(*am, ub, include_member_header)) lb=ub;
            else ub=lb;
            break;
        }
    }

    return ub;
}

/*
 *  Preconditions:
 *      post points to an open/attached encoder
 *      am has open encoders
 *  Postconditions:
 *      post points to an open/attached encoder
 *      am is open/attached if there is room in the archive
 *          otherwise am is not open/attached
 */
bool Archive::Has_Space_For_Part_Of_Member(Archive_Member* am, bool include_member_header){
    DEBUGCXX(debugcxx,"Archive::Has_Space_For_Part_Of_Member");
    bool result;

    if (options_.multi_volume_){
        if (am->Is_Regular())
            result=Has_Space_For_N_Bytes_Of_Member(*am, 1, include_member_header);
        else
            result=Has_Space_For_N_Bytes_Of_Member(*am, 0, include_member_header);

        if (result==false){
            Close_Transitbuf();

            if (am->Is_Regular())
                result=Has_Space_For_N_Bytes_Of_Member(*am, 1, include_member_header);
            else result=Has_Space_For_N_Bytes_Of_Member(*am, 0, include_member_header);
        }
    }
    else result=true;

    return result;
}

/* Store_Part_Of_Member
 * store as much of an Archive_Member as we can. We will either
 * 1) store the entire Archive_Member, or
 * 2) fill up the Archive
 *
 * Preconditions:
 *      this is Attached
 *      am is Attached
 *
 * Postconditions:
 *      this is Attached
 *      am may be unAttached if
 *          1. the archive is a multivolume archive
 *          2. the archive is full
 */

void Archive::Store_Part_Of_Member(Archive_Member *am){
    DEBUGCXX(debugcxx,"Archive::Store_Part_Of_Member");
    if (am->Is_Regular()){
        size_t writable_size=Find_Maximum_Writable_Size(am,false);
        am->Write(writable_size);

        while(options_.multi_volume_ && !am->Is_Empty()){
            if (!Has_Space_For_Part_Of_Member(am,false)) break;

            writable_size=Find_Maximum_Writable_Size(am,false);
            am->Write(writable_size);
        }
    }

}

size_t Archive::Size(void){
    DEBUGCXX(debugcxx,"Archive::Size");

    size_t result;
    result+=archive_trailer.size();

    return result;
}

void Archive::Close(bool last_volume){
    DEBUGCXX(debugcxx,"Archive::Close");
    Close_Transitbuf();

    //archive_trailer=Generate_Archive_Trailer(last_volume,options_.tape_length-last_flush_position-post->encoded_length(0));
    *os << archive_trailer;
    os->flush();

    if (filename!="-")
        ofs.close();
}

void Archive::Close_Transitbuf(void){
    is_attached=false;
}

void Archive::Write_Archive_Header(void){
}

