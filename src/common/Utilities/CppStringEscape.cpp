/*
 * CppStringEscape.cpp
 *
 *  Created on: Jul 24, 2018
 *      Author: dbetz
 */

#include <iomanip>

#include "Utilities/CppStringEscape.hpp"

bool IsXmlChar(char const ch){
	unsigned char c=*reinterpret_cast<unsigned char const *>(&ch);

	if (c==9) return true;
	if (c==10) return true;// newline
	// The presence of #xD in the above production is maintained purely for backward compatibility with the First Edition.
	// As explained in 2.11 End-of-Line Handling, all #xD characters literally present in an XML document are either removed or replaced by #xA characters before any other processing is done.
	// The only way to get a #xD character to match this production is to use a character reference in an entity value literal.
	// ...
	// To simplify the tasks of applications, the XML processor MUST behave as if it normalized all line breaks in external parsed entities (including the document entity) on input, before parsing, by translating both the two-character sequence #xD #xA and any #xD that is not followed by #xA to a single #xA character.
	// if (c==13) return true;
	if (c>=20 && c<=254) return true;

	return false;
}

std::string ToOctal(char const c){
	std::ostringstream oss;

	oss << "\\" << std::oct << std::setw(3) << std::setfill('0') << (int) *reinterpret_cast<unsigned char const *>(&c);

	return oss.str();
}

std::string CppStringEscape(std::string const & plaintext){
	std::string result;

	for(std::string::size_type i=0; i<plaintext.size(); ++i)
		if (IsXmlChar(plaintext[i]))
			result+=plaintext[i];
		else
			result+=ToOctal(plaintext[i]);

	return result;
}
