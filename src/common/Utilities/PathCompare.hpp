/*
 * PathCompare.hpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_UTILITIES_PATHCOMPARE_HPP_
#define SRC_COMMON_UTILITIES_PATHCOMPARE_HPP_

class PathCompare {
public:
	bool operator()(boost::filesystem::path & p1, boost::filesystem::path & p2){ return p1>p2; }
};

#endif /* SRC_COMMON_UTILITIES_PATHCOMPARE_HPP_ */
