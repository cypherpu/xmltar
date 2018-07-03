/*
 * Debug.hpp
 *
 *  Created on: Jul 3, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_DEBUG_HPP_
#define SRC_COMMON_DEBUG_HPP_

class Debug {
	static int depth;
	char const *msg_;
public:
	Debug(char const *msg);
	~Debug();
};

#endif /* SRC_COMMON_DEBUG_HPP_ */
