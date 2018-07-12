/*
 * Debug.hpp
 *
 *  Created on: Jul 3, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_DEBUG_HPP_
#define SRC_COMMON_DEBUG_HPP_

#include <ostream>

namespace betz {

class Debug2 {
	static int depth;
	static int spaces;
	char const *msg_;
public:
	Debug2(char const *msg);
	~Debug2();

	friend std::ostream & operator<<(std::ostream & os, Debug2 const & dbg);
};

}	// betz

#endif /* SRC_COMMON_DEBUG_HPP_ */
