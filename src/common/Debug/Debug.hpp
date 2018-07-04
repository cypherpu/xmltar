/*
 * Debug.hpp
 *
 *  Created on: Jul 3, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_DEBUG_HPP_
#define SRC_COMMON_DEBUG_HPP_

namespace betz {

class Debug {
	static int depth;
	static int spaces;
	char const *msg_;
public:
	Debug(char const *msg);
	~Debug();

	friend std::ostream & operator<<(std::ostream & os, Debug const & dbg);
};

}	// betz

#endif /* SRC_COMMON_DEBUG_HPP_ */
