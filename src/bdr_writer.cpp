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
#include <sys/statvfs.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <errno.h>

#include <json.hpp>

#include "Process/Process.hpp"
#include "Process/Connection.hpp"
#include "Process/Utilities.hpp"

class SCSIDevice {
public:
	int host_;
	int channel_;
	int target_;
	int lun_;
	std::string deviceName_;
	std::string serialNumber_;

	SCSIDevice()
		: host_(-1), channel_(-1), target_(-1), lun_(-1), deviceName_(), serialNumber_(){}

	SCSIDevice(int host, int channel, int target, int lun, std::string serialNumber)
		: host_(host), channel_(channel), target_(target), lun_(lun), deviceName_(), serialNumber_(serialNumber){
		std::filesystem::path scsiHostChannelTargetLunToDeviceNameMappingDirectory("/sys/dev/block");

		for(auto p : std::filesystem::directory_iterator(scsiHostChannelTargetLunToDeviceNameMappingDirectory)){
			if (!std::filesystem::is_symlink(p.path()))
				throw std::runtime_error("Device::Device: "+scsiHostChannelTargetLunToDeviceNameMappingDirectory.string()+" has non-symlink entry");

			// look for cdrom-type devices
			std::filesystem::path::iterator i=p.path().end();

			if (i==p.path().begin())
				throw std::runtime_error("Device::Device: linkTarget has too few path elements");
			else --i;
			if (std::stoi(i->string())!=11) continue;

			std::filesystem::path linkTarget=std::filesystem::read_symlink(p.path());
			i=linkTarget.end();

			if (i==linkTarget.begin())
				throw std::runtime_error("Device::Device: linkTarget has too few path elements");
			else --i;
			if (i->string().substr(0,2)!="sr")
				throw std::runtime_error("Device::Device: linkTarget does not end in srN");
			size_t nCharacters;
			std::stoi(i->string().substr(2),&nCharacters);
			if (nCharacters+2!=i->string().size())
				throw std::runtime_error("Device::Device: linkTarget does not end in srN");
			std::string deviceName="/dev/"+i->string();

			--i;
			if (i==linkTarget.begin())
				throw std::runtime_error("Device::Device: linkTarget has too few path elements");
			if (i->string()!="block")
				throw std::runtime_error("Device::Device: linkTarget does not end in block/srN");

			--i;
			if (i==linkTarget.begin())
				throw std::runtime_error("Device::Device: linkTarget has too few path elements");
			int host1=std::stoi(i->string(),&nCharacters);
			if (i->string().substr(nCharacters)!=":0:0:0")
				throw std::runtime_error("Device::Device: linkTarget does not end in N:0:0:0/block/srN");

			if (host1==host){
				std::cerr << "host=" << host << "  device=" << deviceName << std::endl;
				deviceName_=deviceName;
				return;
			}
		}

		throw std::runtime_error("Device::Device: could not find matching host entry");
	}
};

std::map<std::string,SCSIDevice> IdentifyBDRs(){
	std::map<std::string,SCSIDevice> result;

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
			bool logicalUnitSerialNumberFound=false;
			for(std::string line2; std::getline(inputLines2,line2); ){
				if (line2.find("Feature: 'Logical Unit Serial Number' (current) (persistent)\tSerial: '")!=std::string::npos){
					std::istringstream iss3(line2);
					std::string discard;
					std::string logicalUnitSerialNumber;
					iss3 >> discard >> discard >> discard >> discard >> discard >> discard >> discard >> discard >> logicalUnitSerialNumber;

					if (logicalUnitSerialNumber[0]=='\'') logicalUnitSerialNumber.erase(0,1);
					if (logicalUnitSerialNumber.back()=='\'') logicalUnitSerialNumber.erase(logicalUnitSerialNumber.length()-1,1);
					std::cerr << logicalUnitSerialNumber << std::endl;

					result[logicalUnitSerialNumber]=SCSIDevice(scsibus,scsibus,target,lun,logicalUnitSerialNumber);
					logicalUnitSerialNumberFound=true;
					break;
				}
			}

			if (!logicalUnitSerialNumberFound)
				throw std::runtime_error("IdentifyBDRs: could not find device "+device.str());
		}
	}

	return result;
}

bool trayIsOpen(std::string device){
	int fd;

	if ((fd=open(device.c_str(),O_NONBLOCK))==-1){
		// throw std::runtime_error(std::string("trayIsOpen: could not open: ")+strerror(errno)+" \""+device+"\"");
		std::cerr << std::string("trayIsOpen: could not open: ")+strerror(errno)+" \""+device+"\"" << std::endl;
		std::terminate();
	}

	int result=ioctl(fd, CDROM_DRIVE_STATUS, CDSL_NONE);

	if (result==CDS_TRAY_OPEN)
		return true;	//std::cerr << device << " is open" << std::endl;
	else
		return false;	//std::cerr << device << " is closed" << std::endl;
}

bool mediaAlreadyBurnedOrNoMedia(int host){
	std::istringstream iss1;
	std::ostringstream oss1;

	Process scanbus("/usr/local/bin/cdrecord",{"cdrecord","dev="+std::to_string(host)+",0,0","-minfo","-v"},"cdrecord");
	Chain1(scanbus,iss1,oss1);

	std::istringstream inputLines1(oss1.str());
	for(std::string line1; std::getline(inputLines1,line1); ){
		if (line1.find("disk status:              complete")!=std::string::npos)
			for(std::string line2; std::getline(inputLines1,line2); ){
				if (line2.find("session status:           complete")!=std::string::npos) return true;
			}
		if (line1.find("disk status:              empty")!=std::string::npos)
			for(std::string line2; std::getline(inputLines1,line2); ){
				if (line2.find("session status:           empty")!=std::string::npos) return false;
			}
		if (line1.find("medium not present - tray closed")!=std::string::npos) return true;
	}

	throw std::runtime_error("bdr_writer: medialAlreadyBurned: unknown media status");
}

class LoopDevice {
public:
	struct FileInfo {
		int fd_;
		std::filesystem::path path_;
	};
	std::optional<FileInfo> device_;
	std::optional<FileInfo> backingFile_;

	LoopDevice(){}

	void attach(std::filesystem::path const & backingFilePath){
		if (device_ || backingFile_)
			throw std::logic_error("LoopDevice::attach: loop device already in use");

		int fdLoopControl;

		do {
			long availableLoopDeviceNumber;

			if ((fdLoopControl = open("/dev/loop-control", O_RDWR))==-1)
				throw std::runtime_error("LoopDevice::attach: cannot open /dev/loop-control");

			if ((availableLoopDeviceNumber = ioctl(fdLoopControl, LOOP_CTL_GET_FREE))==-1)
				throw std::runtime_error("LoopDevice::attach: cannot obtain available loop device number");

			std::filesystem::path loopDevicePath="/dev/loop"+std::to_string(availableLoopDeviceNumber);

			device_ = { open(loopDevicePath.c_str(), O_RDWR), loopDevicePath };
		}
		while(device_.value().fd_==-1);

		if (close(fdLoopControl))
			throw std::runtime_error("LoopDevice::attach: "+std::string(strerror(errno)));

		if ((backingFile_ = { open(backingFilePath.c_str(), O_RDWR), backingFilePath }).value().fd_==-1)
	    	throw std::runtime_error("LoopDevice::attach: cannot open backing file "+backingFilePath.string());

	    if (ioctl(device_.value().fd_, LOOP_SET_FD, backingFile_.value().fd_) == -1)
	    	throw std::runtime_error("LoopDevice::attach: cannot attach backing file to loop device");
	}

	void detach(){
		if (!device_ || !backingFile_)
			throw std::runtime_error("LoopDevice::detach: loop device not attached");

	    if (ioctl(device_.value().fd_, LOOP_CLR_FD) == -1)
	    	throw std::runtime_error("LoopDevice::detach: cannot detach backing file from loop device");

	    device_.reset();

	    if (close(backingFile_.value().fd_))
	    	throw std::runtime_error("LoopDevice::detach: cannot close backing file ");

	    backingFile_.reset();
	}

	std::filesystem::path devicePath(){
#if 0
		struct loop_info64 loopInfo;

		if (ioctl(fdLoopDevice_.value(),LOOP_GET_STATUS,&loopInfo)==-1)
			throw std::runtime_error("LoopDevice::filename: could not obtainn loop_info64");

		return loopInfo.lo_file_name;
#endif
		return device_.value().path_;
	}
};

void System(std::string const command){
	int wstatus=system(command.c_str());
	if (wstatus && WIFEXITED(wstatus) && WEXITSTATUS(wstatus))
    	throw std::runtime_error("bdr_writer:System: failed command: \""+command+"\"");
}

void AttachFormatAndMount(LoopDevice & loopDevice, std::filesystem::path const & backingFile, std::filesystem::path mountPath){
	loopDevice.attach(backingFile);
	System("/usr/sbin/mkudffs "+loopDevice.devicePath().string());

	if (mount(loopDevice.devicePath().c_str(),mountPath.c_str(),"udf",0,"")==-1)
		throw std::runtime_error("bdr_writer: AttachFormatAndMount: cannot mount loop device");
}

void BurnImage(SCSIDevice const & scsiDevice, LoopDevice & loopDevice, std::filesystem::path mountPath){
	if (umount(mountPath.c_str())!=0)
		throw std::runtime_error("bdr_writer: could not unmount");
	std::filesystem::path backingFilePath=loopDevice.backingFile_->path_;
	loopDevice.detach();

	for(;;){
		if (!trayIsOpen(scsiDevice.deviceName_))
			if (!mediaAlreadyBurnedOrNoMedia(scsiDevice.host_))
				break;
			else {
				std::cerr << "Insert fresh media in " << scsiDevice.serialNumber_ << std::endl;
				System("/usr/local/bin/cdrecord dev="+std::to_string(scsiDevice.host_)+",0,0 -eject");
			}
		else {
			std::cerr << "Close tray " << scsiDevice.serialNumber_ << std::endl;
			while(trayIsOpen(scsiDevice.deviceName_))
				sleep(10);
		}
	}
	System("/usr/local/bin/cdrecord -v -v dev="+std::to_string(scsiDevice.host_)+",0,0 speed=4 fs=64m "+backingFilePath.string());
}

int main(int argc, char *argv[]){
#if 1
	std::vector<std::string> orderedSerialNumbers {
		"M66IB9H5249",
		"M6BIB9H4809",
		"M62IB9I0856",
		"M66IA3A1811",
		"M66IA395830",
		"M6BIA394645",
		"M69IA3H2850",
		"M6JIA3H2107"
	};
#else
	std::vector<std::string> orderedSerialNumbers {
		"M64IB9I0842",
		"M6IIB9H5304",
	};
#endif
	std::map<std::string,SCSIDevice> serialToScsi=IdentifyBDRs();

	std::ifstream ifs("/home/dbetz/git/Private/xmltar.json");
	auto j=nlohmann::json::parse(ifs);

	std::cerr << "passphrase=\"" << j["passphrase"] << "\"" << std::endl;

	std::filesystem::path readPath("/backup/xmltar_write");
	std::filesystem::path writePath("/backup/xmltar_read");
	std::filesystem::path backingFilePath("/backup/bluray.udf");
	// std::filesystem::path imagePath("/backup/dvd.udf");
	std::filesystem::path mountPath("/backup/bluray_mnt");

	if (!std::filesystem::exists(readPath))
		throw std::runtime_error("bdr_writer: "+readPath.string()+" does not exist");
	if (!std::filesystem::exists(writePath))
		throw std::runtime_error("bdr_writer: "+writePath.string()+" does not exist");
	if (!std::filesystem::exists(backingFilePath))
		throw std::runtime_error("bdr_writer: "+backingFilePath.string()+" does not exist");

	if (!std::filesystem::is_fifo(std::filesystem::symlink_status(readPath)))
		throw std::runtime_error("bdr_writer: "+readPath.string()+" is not a fifo");
	if (!std::filesystem::is_fifo(std::filesystem::symlink_status(writePath)))
		throw std::runtime_error("bdr_writer: "+writePath.string()+" is not a fifo");
	if (!std::filesystem::is_regular_file(std::filesystem::symlink_status(backingFilePath)))
		throw std::runtime_error("bdr_writer: "+backingFilePath.string()+" is not a regular file");
	if (std::filesystem::file_size(backingFilePath)!=25025314816 && std::filesystem::file_size(backingFilePath)!=4706074624)
		throw std::runtime_error("bdr_writer: "+backingFilePath.string()+" is wrong size");

	int fdRead;
	int fdWrite;

	if ((fdRead=open(readPath.c_str(),O_RDWR))==-1)
		throw std::runtime_error("bdr_writer: cannot open fifo "+readPath.string());
	if ((fdWrite=open(writePath.c_str(),O_RDWR))==-1)
		throw std::runtime_error("bdr_writer: cannot open fifo "+writePath.string());

	LoopDevice loopDevice;
	AttachFormatAndMount(loopDevice, backingFilePath, mountPath);

	for(size_t burner=0; ; ){
		char buffer[1024];
		ssize_t nBytes;
		nBytes=read(fdRead,buffer,sizeof(buffer));
		if (nBytes==0) continue; // no process is writing this fifo
		if (nBytes<0)
			throw std::runtime_error("bdr_writer: cannot read fifo");

		std::istringstream iss(std::string(buffer,nBytes));

		std::string action;
		iss >> action;
		if (action=="REQUEST_WRITE"){
			size_t requestedSize;
			iss >> requestedSize;
			struct statvfs buf;
			if (statvfs(mountPath.c_str(),&buf)!=0)
				throw std::runtime_error("bdr_writer: could not statvfs");
			if (requestedSize>buf.f_bavail*buf.f_bsize){
				BurnImage(
					serialToScsi[orderedSerialNumbers[burner]],
					loopDevice,
					mountPath
				);

				if (++burner==orderedSerialNumbers.size()){
					std::cerr << "****** Press <enter> to eject discs" << std::endl;
					std::string response;
					std::getline(std::cin,response);

					for(size_t i=0; i<burner; ++i){
						System("/usr/local/bin/cdrecord dev="+std::to_string(serialToScsi[orderedSerialNumbers[i]].host_)+",0,0 -eject");
					}
					burner=0;
				}

				AttachFormatAndMount(loopDevice,backingFilePath,mountPath);
			}

			nBytes=write(fdWrite,"CONTINUE",8);
			if (nBytes<0)
				throw std::runtime_error("bdr_writer: cannot write fifo");
			if (nBytes!=8)
				throw std::runtime_error("bdr_writer: partial write fifo");
			continue;
		}
		else if (action=="FINISHED"){
			BurnImage(
				serialToScsi[orderedSerialNumbers[burner]],
				loopDevice,
				mountPath
			);

			std::cerr << "****** Press <enter> to eject final discs" << std::endl;
			std::string response;
			std::getline(std::cin,response);

			for(size_t i=0; i<=burner; ++i){
				System("/usr/local/bin/cdrecord dev="+std::to_string(serialToScsi[orderedSerialNumbers[i]].host_)+",0,0 -eject");
			}

			break;
		}
	}
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
