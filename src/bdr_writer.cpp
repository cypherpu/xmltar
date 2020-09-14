/*
 * bdr_writer.cpp
 *
 *  Created on: Sep 11, 2020
 *      Author: dbetz
 */

#include <map>
#include <sstream>
#include <filesystem>
#include <fstream>

#include <fcntl.h>
#include <linux/loop.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>

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

	std::cerr << "passphrase=\"" << j["passphrase"] << "\"" << std::endl;

	std::filesystem::path readPath("/backup/xmltar_write");
	std::filesystem::path writePath("/backup/xmltar_read");
	std::filesystem::path imagePath("/backup/bluray.udf");
	std::filesystem::path mountPath("/backup/bluray_mnt");

	if (!std::filesystem::exists(readPath))
		throw std::runtime_error("bdr_writer: "+readPath.string()+" does not exist");
	if (!std::filesystem::exists(writePath))
		throw std::runtime_error("bdr_writer: "+writePath.string()+" does not exist");
	if (!std::filesystem::exists(imagePath))
		throw std::runtime_error("bdr_writer: "+imagePath.string()+" does not exist");

	if (!std::filesystem::is_fifo(std::filesystem::symlink_status(readPath)))
		throw std::runtime_error("bdr_writer: "+readPath.string()+" is not a fifo");
	if (!std::filesystem::is_fifo(std::filesystem::symlink_status(writePath)))
		throw std::runtime_error("bdr_writer: "+writePath.string()+" is not a fifo");
	if (!std::filesystem::is_regular_file(std::filesystem::symlink_status(imagePath)))
		throw std::runtime_error("bdr_writer: "+imagePath.string()+" is not a regular file");
	if (std::filesystem::file_size(imagePath)!=25025314816)
		throw std::runtime_error("bdr_writer: "+imagePath.string()+" is wrong size");

	int fdRead;
	int fdWrite;

	if ((fdRead=open(readPath.c_str(),O_RDWR))==-1)
		throw std::runtime_error("bdr_writer: cannot open fifo "+readPath.string());
	if ((fdWrite=open(writePath.c_str(),O_RDWR))==-1)
		throw std::runtime_error("bdr_writer: cannot open fifo "+writePath.string());

	// losetup
    int fdLoopControl, fdLoopDevice, fdBackingFile;
    long availableLoopDeviceNumber;;
    std::filesystem::path loopDevicePath;

    if ((fdLoopControl = open("/dev/loop-control", O_RDWR))==-1)
    	throw std::runtime_error("bdr_writer: cannot open /dev/loop-control");

    if ((availableLoopDeviceNumber = ioctl(fdLoopControl, LOOP_CTL_GET_FREE))==-1)
    	throw std::runtime_error("bdr_writer: cannot obtain available loop device number");

    loopDevicePath="/dev/loop"+std::to_string(availableLoopDeviceNumber);

    if ((fdLoopDevice = open(loopDevicePath.c_str(), O_RDWR))==-1)
    	throw std::runtime_error("bdr_writer: cannot open loop device "+loopDevicePath.string());

    if ((fdBackingFile = open(imagePath.c_str(), O_RDWR))==-1)
    	throw std::runtime_error("bdr_writer: cannot open backing file "+imagePath.string());

    if (ioctl(fdLoopDevice, LOOP_SET_FD, fdBackingFile) == -1)
    	throw std::runtime_error("bdr_writer: cannot attach backing file to loop device");

    if (mount(loopDevicePath.c_str(),mountPath.c_str(),"udf",0,"")==-1)
    	throw std::runtime_error("bdr_writer: cannot mount loop device");
/*

	/usr/bin/dd if=/dev/zero of=/backup/bluray.udf count=25025314816 iflag=count_bytes
	/usr/sbin/losetup /dev/loop0 /backup/bluray.udf
	/usr/sbin/mkudffs /dev/loop0
	/usr/bin/mkdir /backup/bluray_mnt
	/usr/bin/mount /backup/bluray.udf /backup/bluray_mnt

	/usr/bin/rm -f /backup/xmltar_in
	/usr/bin/rm -f /backup/xmltar_out

	/usr/bin/mkfifo /backup/xmltar_in
	/usr/bin/mkfifo /backup/xmltar_out

	/usr/local/bin/cdrecord -vvv dev=12,0,0 speed=4 fs=64m /backup/bluray.udf
	*/
}
