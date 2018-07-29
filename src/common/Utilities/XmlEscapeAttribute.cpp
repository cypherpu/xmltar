/*
 * XMLEscapeAttribute.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dbetz
 */

#include <stdexcept>

#include "XmlEscapeAttribute.hpp"

struct EntityMap {
	char c_;
	std::string entity_;
};

EntityMap entities[]
{
	{ '&',	"&amp;" },
	{ '\"',	"&quot;" },
	{ '\'',	"&apos;" },
	{ '<',	"&lt;" },
	{ '>',	"&gt;" },
};

std::string XmlEscapeAttribute(std::string const data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t i = 0; i != data.size(); ++i) {
        switch(data[i]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[i], 1); break;
        }
    }

    return buffer;
}

std::string XmlUnscapeAttribute(std::string const data) {
    std::string buffer;
    buffer.reserve(data.size());

    for(size_t i = 0; i != data.size(); )
    	if (data[i]=='&'){
    		size_t j;
    		for(j=0; j<sizeof(entities); ++j)
    			if (data.size()>=i+entities[j].entity_.size() && !data.compare(0,entities[j].entity_.size(),entities[j].entity_)){
    				buffer.append(&entities[i].c_,1);
    				break;
    			}

    		if (j==sizeof(entities))
    			throw std::logic_error("XMLUnescapeAttribute: fell unknown predefined character entity reference");
    	}
    	else buffer.append(&data[i++],1);

    return buffer;
}