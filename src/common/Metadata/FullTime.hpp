/*
 * FullTime.hpp
 *
 *  Created on: Nov 5, 2017
 *      Author: dbetz
 */

#ifndef SRC_COMMON_METADATA_FULLTIME_HPP_
#define SRC_COMMON_METADATA_FULLTIME_HPP_

struct FullTime {
	time_t posix_time_;
	std::string formatted_time_;
};

#endif /* SRC_COMMON_METADATA_FULLTIME_HPP_ */
