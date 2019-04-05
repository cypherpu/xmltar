/*
 * XmltarArchiveCreateMultiVolume.cpp
 *
 *  Created on: Feb 24, 2019
 *      Author: dbetz
 */

#include <iomanip>
#include <fstream>
#include <iostream>

#include "Xmltar/XmltarArchiveCreateMultiVolume.hpp"
#include "Debug2/Debug2.hpp"

XmltarArchiveCreateMultiVolume::XmltarArchiveCreateMultiVolume(
		XmltarOptions & opts,
		std::string filename,
		std::shared_ptr<XmltarMemberCreate> & nextMember
	)
	: XmltarArchive(opts,filename,/*volumeNumber,*/nextMember)
{
	betz::Debug2 dbg("XmltarArchiveCreateMultiVolume::XmltarArchiveCreateMultiVolume");
	std::shared_ptr<Transform> archiveCompression(options_.archiveCompression_.get()->clone());

	if (!options_.tape_length_)
		throw std::runtime_error("XmltarArchive::XmltarArchive: --tape-length must be specified when creating multi-volume archive");

	std::ofstream ofs(filename_);
	std::string compressedArchiveHeader=CompressedArchiveHeader(filename_,options_.current_volume_);
	std::string compressedArchiveTrailer=CompressedArchiveTrailer();

	if (options_.tape_length_.get()<compressedArchiveHeader.size()+compressedArchiveTrailer.size())
		throw std::logic_error("XmltarArchive::XmltarArchive: archive too small to hold header and trailer of archive member");

	ofs << compressedArchiveHeader;
	size_t committedBytes=compressedArchiveHeader.size();
	size_t pendingBytes=compressedArchiveTrailer.size();

	std::cerr << dbg << "::compressedArchiveHeader.size()= " << compressedArchiveHeader.size() << std::endl;
	std::cerr << dbg << "::compressedArchiveTrailer.size()=" << compressedArchiveTrailer.size() << std::endl;

	archiveCompression->OpenCompression();

	if (!options_.filesToBeIncluded_.empty() && !nextMember_)
		nextMember_=NextMember();

	for(bool firstPass=true; nextMember_; firstPass=false){
		std::cerr << dbg << ": ##########" << std::endl;
		std::cerr << dbg << ": committedBytes=" << std::right << std::setw(8) << committedBytes << std::endl;
		std::cerr << dbg << ": pendingBytes  =" << std::right << std::setw(8) << pendingBytes << std::endl;
		std::cerr << dbg << ": sum           =" << std::right << std::setw(8) << committedBytes+pendingBytes << std::endl;
		std::cerr << dbg << ": file=" << nextMember_->filepath() << std::endl;

		if (nextMember_->isDirectory()){
			if (nextMember_->CanArchiveDirectory(committedBytes, pendingBytes, archiveCompression)){
				std::string tmp=nextMember_->MemberHeader()+nextMember_->MemberTrailer();
				std::string compressedDirectoryMember
					= options_.archiveMemberCompression_->CompressString(
							tmp
						);
				ofs << archiveCompression->ForceWrite(compressedDirectoryMember);
				nextMember_=NextMember();
				pendingBytes=archiveCompression->MaximumCompressedtextSizeGivenPlaintextSize(archiveCompression->WriteCount())+compressedArchiveTrailer.size();
				std::cerr << dbg << ": dir: bytes written=" << tmp.size() << " " << compressedDirectoryMember.size() << std::endl;
			}
			else if (firstPass)
				throw std::logic_error("XmltarArchive::XmltarArchive: archive too small to hold directory archive member");
			else {
				ofs << archiveCompression->ForceWriteAndClose("");
				ofs.flush();
				committedBytes+=archiveCompression->ReadCount();
				pendingBytes=compressedArchiveTrailer.size();
				if (nextMember_->CanArchiveDirectory(committedBytes, pendingBytes, archiveCompression)){
					archiveCompression.reset(archiveCompression->clone());
					archiveCompression->OpenCompression();
				}
				else {
					std::string tmp=CompressedArchiveTrailer(options_.tape_length_.get()-committedBytes);
					std::cerr << dbg << ": directory tmp.size()=" << tmp.size() << std::endl;
					ofs << tmp;
					ofs.flush();
					return;
				}
			}
		}
		else if (nextMember_->isSymLink()){
			if (nextMember_->CanArchiveSymLink(committedBytes, pendingBytes, archiveCompression)){
				std::string tmp=nextMember_->MemberHeader()+nextMember_->MemberTrailer();
				std::string compressedDirectoryMember
					= options_.archiveMemberCompression_->CompressString(
							tmp
						);
				ofs << archiveCompression->ForceWrite(compressedDirectoryMember);
				nextMember_=NextMember();
				pendingBytes=archiveCompression->MaximumCompressedtextSizeGivenPlaintextSize(archiveCompression->WriteCount())+compressedArchiveTrailer.size();
				std::cerr << dbg << ": dir: bytes written=" << tmp.size() << " " << compressedDirectoryMember.size() << std::endl;
			}
			else if (firstPass)
				throw std::logic_error("XmltarArchive::XmltarArchive: archive too small to hold directory archive member");
			else {
				ofs << archiveCompression->ForceWriteAndClose("");
				ofs.flush();
				committedBytes+=archiveCompression->ReadCount();
				pendingBytes=compressedArchiveTrailer.size();
				if (nextMember_->CanArchiveSymLink(committedBytes, pendingBytes, archiveCompression)){
					archiveCompression.reset(archiveCompression->clone());
					archiveCompression->OpenCompression();
				}
				else {
					std::string tmp=CompressedArchiveTrailer(options_.tape_length_.get()-committedBytes);
					std::cerr << dbg << ": directory tmp.size()=" << tmp.size() << std::endl;
					ofs << tmp;
					ofs.flush();
					return;
				}
			}
		}
		else if (nextMember_->isRegularFile()){
			std::cerr << "********** isRegularFile" << std::endl;
			size_t numberOfFileBytesThatCanBeArchived=nextMember_->NumberOfFileBytesThatCanBeArchived(committedBytes,pendingBytes,archiveCompression);
			std::cerr << dbg << ": archiving " << numberOfFileBytesThatCanBeArchived << " of " << nextMember_->filepath().string() << std::endl;
			if (numberOfFileBytesThatCanBeArchived==0){
				if (firstPass)
					throw std::logic_error("XmltarArchive::XmltarArchive: archive too small to hold even 1 char of archive member");
				else {	// close off this archiveCompression to free up space
					ofs << archiveCompression->ForceWriteAndClose("");
					committedBytes+=archiveCompression->ReadCount();
					pendingBytes=compressedArchiveTrailer.size();
					numberOfFileBytesThatCanBeArchived=nextMember_->NumberOfFileBytesThatCanBeArchived(committedBytes,pendingBytes,archiveCompression);
					std::cerr << dbg << ": committedBytes=" << committedBytes << std::endl;
					std::cerr << dbg << ": pendingBytes=" << pendingBytes << std::endl;
					std::cerr << dbg << ": numberOfFileBytesThatCanBeArchived=" << numberOfFileBytesThatCanBeArchived << std::endl;
					if (numberOfFileBytesThatCanBeArchived==0){
						if (committedBytes+compressedArchiveTrailer.size()<=options_.tape_length_.get()){
							std::string tmp=CompressedArchiveTrailer(options_.tape_length_.get()-committedBytes);
							std::cerr << dbg << ": tmp.size()=" << tmp.size() << std::endl;
							ofs << tmp;
							ofs.flush();

							return;
						}
						else
							throw std::logic_error("XmltarARchive::XmltarArchive: overflow");
					}
					else {
						archiveCompression.reset(archiveCompression->clone());
						archiveCompression->OpenCompression();
					}
				}
			}

			nextMember_->write(archiveCompression,numberOfFileBytesThatCanBeArchived,ofs);
			pendingBytes=archiveCompression->MaximumCompressedtextSizeGivenPlaintextSize(archiveCompression->WriteCount())+compressedArchiveTrailer.size();
			if (nextMember_->IsComplete())
				nextMember_=NextMember();
			else
				nextMember_->RecalculateMemberHeader();
		}

	}

	ofs << archiveCompression->ForceWriteAndClose("");
	committedBytes+=archiveCompression->ReadCount();
	pendingBytes=compressedArchiveTrailer.size();
	std::cerr << dbg << ": committedBytes=" << committedBytes << std::endl;
	std::cerr << dbg << ": pendingBytes=" << pendingBytes << std::endl;

	if (committedBytes+compressedArchiveTrailer.size()<=options_.tape_length_.get()){
		std::string tmp=CompressedArchiveTrailer(options_.tape_length_.get()-committedBytes);
		std::cerr << dbg << ": tmp=" << tmp.size() << std::endl;
		ofs << tmp;
		return;
	}
	else
		throw std::logic_error("XmltarARchive::XmltarArchive: overflow");
}
