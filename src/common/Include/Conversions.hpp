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

#ifndef Conversions_hpp_
#define Conversions_hpp_

int String_To_Integer(std::string);
std::string To_Decimal_Int(int);
std::string To_Octal_Int(int);
std::string String_To_XML_Attribute_Value(std::string);
std::string XML_Attribute_Value_To_String(std::string);
std::string To_Local_Time(time_t);

#endif /*Conversions_hpp_*/
