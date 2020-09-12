/*
 * bdr_writer.cpp
 *
 *  Created on: Sep 11, 2020
 *      Author: dbetz
 */

#include <map>
#include <sstream>

#include <json.hpp>

#include "Process/Process.hpp"
#include "Process/Connection.hpp"
#include "Process/Utilities.hpp"

class Device {
public:
	int scsibus_;
	int target_;
	int lun_;

	Device()
		: scsibus_(-1), target_(-1), lun_(-1){}

	Device(int scsibus, int target, int lun)
		: scsibus_(scsibus), target_(target), lun_(lun){}
};

std::map<std::string,Device> IdentifyBDRs(){
	std::map<std::string,Device> result;

	std::istringstream iss1;
	std::ostringstream oss1;

	Process scanbus("/usr/local/bin/cdrecord",{"cdrecord","-scanbus"},"cdrecord");
	Chain1(scanbus,iss1,oss1);

	std::istringstream inputLines1(oss1.str());
	for(std::string line1; std::getline(inputLines1,line1); ){
		if (line1.find("BD-RE")!=std::string::npos && line1.find("Removable CD-ROM")!=std::string::npos){
			std::istringstream inputLine1(line1);
			int scsibus, target, lun;
			char c;
			inputLine1 >> scsibus >> c >> target >> c >> lun;
			std::cerr << "scsibus=" << scsibus << " target=" << target << " lun=" << lun << std::endl;

			std::ostringstream device;
			device << "dev=" << scsibus << "," << target << "," << lun;
			Process prcap("/usr/local/bin/cdrecord",{"cdrecord",device.str().c_str(),"-prcap"},"cdrecord");
			std::istringstream iss2;
			std::ostringstream oss2;
			Chain1(prcap,iss2,oss2);

			std::istringstream inputLines2(oss2.str());
			for(std::string line2; std::getline(inputLines2,line2); ){
				if (line2.find("Feature: 'Logical Unit Serial Number' (current) (persistent)\tSerial: '")!=std::string::npos){
					std::istringstream iss3(line2);
					std::string discard;
					std::string logicalUnitSerialNumber;
					iss3 >> discard >> discard >> discard >> discard >> discard >> discard >> discard >> discard >> logicalUnitSerialNumber;

					if (logicalUnitSerialNumber[0]=='\'') logicalUnitSerialNumber.erase(0,1);
					if (logicalUnitSerialNumber.back()=='\'') logicalUnitSerialNumber.erase(logicalUnitSerialNumber.length()-1,1);
					std::cerr << logicalUnitSerialNumber << std::endl;

					result[logicalUnitSerialNumber]=Device(scsibus,target,lun);
				}
			}
		}
	}

	return result;
}


int main(int argc, char *argv[]){
	std::vector<std::string> orderedSerialNumbers {
		"M64IB9I0842",
		"M6IIB9H5304",
		"M62IB9I0856",
		"M66IA3A1811",
		"M66IA395830",
		"M6BIA394645",
		"M69IA3H2850",
		"M6JIA3H2107"
	};

	std::map<std::string,Device> serialToScsi=IdentifyBDRs();

	std::ifstream ifs("/home/dbetz/git/Private/xmltar.json");
	auto j=nlohmann::json::parse(ifs);


	std::cerr << "TeachGoogleClientId=\"" << teachGoogleClientId << "\"" << std::endl;
	std::cerr << "TeachGoogleClientSecret=\"" << teachGoogleClientSecret << "\"" << std::endl;
	std::cerr << "TeachGoogleCallback=\"" << teachGoogleCallback << "\"" << std::endl;
	std::cerr << "ServerDomainAndPort=\"" << serverDomainAndPort << "\"" << std::endl;

}
