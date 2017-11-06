/*
 * Metadata.hpp
 *
 *  Created on: Nov 4, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_METADATA_METADATA_HPP_
#define SRC_COMMON_METADATA_METADATA_HPP_

#include <string>

#include "Xmltar/XmltarOptions.hpp"

class Metadata {
public:
	Metadata(){}
	virtual ~Metadata(){}

	virtual std::string toXML(XmltarOptions & options);
	virtual void fromXML();
};


#endif /* SRC_COMMON_METADATA_METADATA_HPP_ */
