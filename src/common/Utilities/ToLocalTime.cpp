/*
 * ToLocalTime.cpp
 *
 *  Created on: Feb 24, 2018
 *      Author: dbetz
 */

#include "Utilities/ToLocalTime.hpp"

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"

std::string ToLocalTime(time_t t){
	boost::posix_time::ptime pt=boost::posix_time::from_time_t(t);
	boost::posix_time::ptime local_time = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(pt);

	return boost::posix_time::to_simple_string(local_time);
}
