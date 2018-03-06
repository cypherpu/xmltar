/*
 * ToLocalTime.hpp
 *
 *  Created on: Feb 24, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_UTILITIES_TOLOCALTIME_HPP_
#define SRC_COMMON_UTILITIES_TOLOCALTIME_HPP_

#include <string>

extern "C" {
#include <time.h>
}

std::string ToLocalTime(time_t t);

#endif /* SRC_COMMON_UTILITIES_TOLOCALTIME_HPP_ */
