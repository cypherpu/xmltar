/*
 * Utilities.hpp
 *
 *  Created on: Feb 9, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_PROCESS_UTILITIES_HPP_
#define SRC_COMMON_PROCESS_UTILITIES_HPP_

#include <iostream>

#include "Process/Process.hpp"

void Chain1(Process & p, std::istream & is, std::ostream & os);
void Chain2(Process & p, Process & q, std::istream & is, std::ostream & os);
void Chain3(Process & p, Process & q, Process & r, std::istream & is, std::ostream & os);

#endif /* SRC_COMMON_PROCESS_UTILITIES_HPP_ */
