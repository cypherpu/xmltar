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

#include <string>


#include "Include/Conversions.hpp"

int String_To_Integer(std::string s){
	int result=0;

	for(std::string::iterator i=s.begin(); i!=s.end(); ++i)
		if (!std::isdigit(*i))
			throw "String_To_Integer: cannot convert "+s+" to integer";
		else result=10*result+*i-'0';

	return result;
}

#if 0
std::string String_To_XML_Attribute_Value(std::string s){
	std::string result;

	for(std::string::iterator i=s.begin(); i!=s.end(); ++i)
		if (*i=='"') result+="&quot;";
		else if (*i=='&') result+="&amp;";
		else if (*i=='<') result+="&lt;";
		else if (*i=='\\') result+="\\\\";
		else if (*i==0x7E) result+="\\376";
		else if (*i==0x7F) result+="\\377";
		else if (((unsigned char) *i)>0x7F){
			unsigned char r;

			r=((unsigned char) *i)%64;

			result+=('0'+((unsigned char) *i)/64);
			result+=('0'+((unsigned char) r)/8);
			result+=('0'+((unsigned char) r)%8);
		}
		else if (*i<0x20){
			result+="\\0";
			result+=('0'+((unsigned char) *i)/8);
			result+=('0'+((unsigned char) *i)%8);
		}
		else result+=*i;

	return result;
}

std::string XML_Attribute_Value_To_String(std::string s){
	std::string result;

	for(std::string::iterator i=s.begin(); i!=s.end(); ++i)
		if (*i=='\\'){
			++i;

			if (i==s.end()) throw "XML_Attribute_Value_To_String: premature end of attribute value";

			if (*i=='\\') result+='\\';
			else {
				unsigned char c=0;

				if (*i<'0' || *i >'7')
					throw "XML_Attribute_Value_To_String: invalid C escape sequence 1 "+s;
				else c=8*c+*i-'0';

				++i;
				if (*i<'0' || *i >'7' || i==s.end())
					throw "XML_Attribute_Value_To_String: invalid C escape sequence 2 "+s;
				else c=8*c+*i-'0';

				++i;
				if (*i<'0' || *i >'7' || i==s.end())
					throw "XML_Attribute_Value_To_String: invalid C escape sequence 3"+s;
				else c=8*c+*i-'0';
			}
		}
		else result+=*i;

	return result;
}
#endif
