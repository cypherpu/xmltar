/*
 * DMap.hpp
 *
 *  Created on: Jun 16, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_TRANSFORM_DMAP_HPP_
#define SRC_COMMON_TRANSFORM_DMAP_HPP_

#include <map>

extern std::map<char const *, std::map<char const *, std::map<int,std::tuple<int,int>>>>
dMap;

#endif /* SRC_COMMON_TRANSFORM_DMAP_HPP_ */
