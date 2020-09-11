/*
 * bdr_writer.cpp
 *
 *  Created on: Sep 11, 2020
 *      Author: dbetz
 */

#include <sstream>

#include "Process/Process.hpp"
#include "Process/Connection.hpp"
#include "Process/Utilities.hpp"

void IdentifyBDRs(){
	std::istringstream iss;
	std::ostringstream oss;

	Process scanbus("/usr/local/bin/cdrecord",{"cdrecord","-scanbus"},"cdrecord");
	Chain1(scanbus,iss,oss);

	std::cerr << oss.str() << std::endl;
}


int main(int argc, char *argv[]){
	IdentifyBDRs();
}
