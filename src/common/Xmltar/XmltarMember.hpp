/*
 * XmltarMember.hpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARMEMBER_HPP_
#define SRC_COMMON_XMLTAR_XMLTARMEMBER_HPP_

#include "Options/XmltarOptions.hpp"

class XmltarMember {
	XmltarOptions const  & options_;
	boost::filesystem::path const & filepath_;
	bool includeMetadata_;

public:
	XmltarMember(XmltarOptions const & options, boost::filesystem::path const & filepath, bool includeMetadata)
		: options_(options), filepath_(filepath), includeMetadata_(includeMetadata) {}

	std::string Header();
	std::string Trailer();
};



#endif /* SRC_COMMON_XMLTAR_XMLTARMEMBER_HPP_ */
