
/*
 * XmltarArchive.cpp
 *
 *  Created on: Nov 21, 2017
 *      Author: dbetz
 */

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <pwd.h>
#include <grp.h>
}

#include <boost/circular_buffer.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int.hpp>

#include "Xmltar/XmltarArchive.hpp"
#include "Xmltar/XmltarMemberCreate.hpp"
#include "Utilities/ToHexDigit.hpp"
#include "Utilities/IsPrefixPath.hpp"
#include "Utilities/Glob.hpp"

#include "XmlParser/XmlParser.hpp"

#include "../Debug2/Debug2.hpp"

class NonDeterministicRNG : public boost::random::random_device {
public:
	NonDeterministicRNG()
		: boost::random::random_device("/dev/urandom")				// TODO: change this to /dev/random once we have a better entropy source
	{}
};

class CBEntry {
public:
	char c_;
	std::ios::off_type offset_;

	CBEntry(char c, std::ios::off_type offset)
		: c_(c), offset_(offset){}

	CBEntry(std::fstream & iofs){
		offset_=iofs.tellg();
		iofs.get(c_);
	}
};

XmltarArchive::XmltarArchive(
	XmltarOptions const & opts,
	XmltarGlobals & globals,
	std::string filename,
	unsigned int volumeNumber,
	std::unique_ptr<XmltarMemberCreate> & nextMember
)
	: options_(opts), globals_(globals), filename_(filename), volumeNumber_(volumeNumber), nextMember_(nextMember)
{
	betz::Debug2 dbg("XmltarArchive::XmltarArchive");
}

XmltarArchive::XmltarArchive(XmltarOptions const & opts, XmltarGlobals & globals, std::string filename, std::unique_ptr<XmltarMemberCreate> & nextMember)
: options_(opts), globals_(globals), filename_(filename), volumeNumber_(0), nextMember_(nextMember)
{
	if (options_.operation_.get()==XmltarOptions::Operation::EXTRACT){
		if (options_.multi_volume_){
			std:: ifstream ifs(filename);

			if (!ifs)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" cannot be read");
#if 0
			char smallBuf[5];
			ifs.read(smallBuf,sizeof(smallBuf));
			if (!ifs)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" error or EOF");
			if (ifs.gcount()!=5)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" too short");
			std::string bufString(smallBuf,sizeof(smallBuf));

			std::vector<std::shared_ptr<Transform>> transformations;
			if (bufString==TransformIdentity::StaticHeaderMagicNumber("<?xml"))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformIdentity>("archiveCompression"));
			else if (bufString.substr(2)==TransformGzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformGzip>("archiveCompression"));
			else if (bufString.substr(3)==TransformBzip2::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformBzip2>("archiveCompression"));
			else if (bufString.substr(5)==TransformLzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformLzip>("archiveCompression"));

			transformations[0]->OpenDecompression();
			std::string readString=transformations[0]->ForceWrite(bufString);
			char buffer[2014];
			while(ifs && readString.size()<5){
				ifs.read(buffer,sizeof(buffer));
				readString=transformations[0]->ForceWrite(std::string(buffer,ifs.gcount()));
			}

			if (readString.size()<5)
				throw std::runtime_error("XmltarArchive::XmltarArchive: readString.size()<5");
			if (readString==TransformIdentity::StaticHeaderMagicNumber("<?xml"))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformIdentity>("memberCompression"));
			else if (readString.substr(2)==TransformGzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformGzip>("memberCompression"));
			else if (readString.substr(3)==TransformBzip2::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformBzip2>("memberCompression"));
			else if (readString.substr(5)==TransformLzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformLzip>("memberCompression"));
#endif
		}
		else {
#if 0
			std:: ifstream ifs(filename);

			if (!ifs)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" cannot be read");

			char smallBuf[5];
			ifs.read(smallBuf,sizeof(smallBuf));
			if (!ifs)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" error or EOF");
			if (ifs.gcount()!=5)
				std::runtime_error("XmltarArchive::XmltarArchive: "+filename+" too short");
			std::string bufString(smallBuf,sizeof(smallBuf));
			// std::cerr << "bufString=" << bufString << std::endl;

			std::vector<std::shared_ptr<Transform>> transformations;
			if (bufString==TransformIdentity::StaticHeaderMagicNumber("<?xml"))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformIdentity>("archiveCompression"));
			else if (bufString.substr(2)==TransformGzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformGzip>("archiveCompression"));
			else if (bufString.substr(3)==TransformBzip2::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformBzip2>("archiveCompression"));
			else if (bufString.substr(5)==TransformLzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformLzip>("archiveCompression"));

			transformations[0]->OpenDecompression();
			std::string readString=transformations[0]->ForceWrite(bufString);
			char buffer[2014];
			while(ifs && readString.size()<5){
				ifs.read(buffer,sizeof(buffer));
				readString+=transformations[0]->ForceWrite(std::string(buffer,ifs.gcount()));
			}

			if (readString.size()<5)
				throw std::runtime_error("XmltarArchive::XmltarArchive: readString.size()<5");
			if (readString==TransformIdentity::StaticHeaderMagicNumber("<?xml"))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformIdentity>("memberCompression"));
			else if (readString.substr(2)==TransformGzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformGzip>("memberCompression"));
			else if (readString.substr(3)==TransformBzip2::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformBzip2>("memberCompression"));
			else if (readString.substr(5)==TransformLzip::StaticHeaderMagicNumber(""))	// FIXME - C++20 starts_with
				transformations.push_back(std::make_shared<TransformLzip>("memberCompression"));

			// std::cerr << "readString=" << readString << std::endl;

			XmltarMultiVolumeXmlHandler archiveParser(*this);
			archiveParser.Parse(readString,false);

			while(ifs){
				ifs.read(buffer,sizeof(buffer));
				readString=transformations[1]->ForceWrite(transformations[0]->ForceWrite(std::string(buffer,ifs.gcount())));
				archiveParser.Parse(readString,false);
			}

			readString=transformations[1]->ForceWriteAndClose(transformations[0]->ForceWriteAndClose(""));
			// std::cerr << readString;
			archiveParser.Parse(readString,true);
#endif
		}
	}
}

void XmltarArchive::NextMember(){
	std::cerr << "XmltarArchive::NextMember(): entering" << std::endl;
	nextMember_.reset();

	if(globals_.filesToBeIncluded_.empty() && !globals_.globsToBeIncluded_.empty()){
		std::cerr << "XmltarArchive::NextMember(): replenish files to be included" << std::endl;
		while(globals_.filesToBeIncluded_.empty() && !globals_.globsToBeIncluded_.empty()){
			std::vector<std::string> tmp=BashGlob({globals_.globsToBeIncluded_[0]});
			for(auto & i : tmp)
				globals_.filesToBeIncluded_.push(std::filesystem::path(i));

			globals_.globsToBeIncluded_.erase(globals_.globsToBeIncluded_.begin());
		}

		if (globals_.filesToBeIncluded_.empty())
			return;

		if (globals_.snapshot_.get()!=nullptr)
			globals_.snapshot_->NewTemporarySnapshotFile();
	}

	globals_.filesToBeExcludedTruncated_=globals_.filesToBeExcludedComplete_;
	/*
	 * Files to be included in the archive are archived in command-line order.
	 * As files are included, we would like to erase excluded paths which
	 * could not possibly be relevant to any further included paths.
	 * as possible. It is not sufficient to merely erase excluded paths which
	 * are less than the current included path - we should not erase the excluded
	 * path "/bin" just because it is less than "/bin/foo"; we must also ensure
	 * the excluded path is not a path prefix of the included path.
	 */
	std::filesystem::path filepath;

	if (globals_.filesToBeIncluded_.empty()){
		nextMember_.reset(nullptr);
		return;
	}
	for( ; !globals_.filesToBeIncluded_.empty(); ){
		filepath=globals_.filesToBeIncluded_.top();
		globals_.filesToBeIncluded_.pop();
		std::cerr	<< "############ XmltarArchiveCreateSingleVolume::NextMember: "
					<< "considering " << filepath.string() << std::endl;
		while(!globals_.filesToBeExcludedTruncated_.empty() &&
				globals_.filesToBeExcludedTruncated_.top()<filepath &&
			!IsPrefixPath(globals_.filesToBeExcludedTruncated_.top(),filepath)){
			std::cerr	<< "############ XmltarArchiveCreateSingleVolume::NextMember: "
						<< "discarding exclude file " << globals_.filesToBeExcludedTruncated_.top().string() << std::endl;
			globals_.filesToBeExcludedTruncated_.pop();
		}

		std::cerr	<< "############ XmltarArchiveCreateSingleVolume::NextMember: "
					<< "filesToBeExcludedTruncated_.top()=" << (globals_.filesToBeExcludedTruncated_.size()?globals_.filesToBeExcludedTruncated_.top().string():"") << std::endl;

		if (globals_.filesToBeExcludedTruncated_.empty())
			break;
		if (IsPrefixPath(globals_.filesToBeExcludedTruncated_.top(),filepath)){
			std::cerr	<< "########### XmltarArchiveCreateSingleVolume::NextMember: "
						<< "discarding include file " << filepath << std::endl;
			continue;
		}
		else break;
	}

	std::cerr << "NextMember=" << filepath << std::endl;
	std::filesystem::file_status f_stat=std::filesystem::symlink_status(filepath);

	if (std::filesystem::is_directory(f_stat)){
		for(auto & p : std::filesystem::directory_iterator(filepath) ){
			globals_.filesToBeIncluded_.push(p);
		}
	}

	nextMember_.reset(new XmltarMemberCreate(options_,globals_,filepath));
}

PartialFileRead XmltarArchive::append(unsigned int volumeNumber)
{
#if 0
	// we have to back-track to find the end of the last member in the archive
	boost::format fmt(options_.base_xmltar_file_name_.get());
	fmt % volumeNumber;
	std::string filename=str(fmt);

	if (!std::filesystem::exists(filename)){
		return create(volumeNumber);
	}
	else {
		std::fstream iofs(filename);
		std::string magic=HeaderMagicNumber(options_.archiveMemberCompression_.get(),"<padding>");

		// create a list of possible trailer starts
		std::vector<std::ios::pos_type> positions;
		boost::circular_buffer<CBEntry> cb(magic.size());
		for(;;){
			while(iofs)
				if (cb.size()<magic.size())
					cb.push_back(CBEntry(iofs));
			if (!iofs && cb.size()<magic.size()) break;

			int matchLen;
			for( matchLen=0; matchLen<magic.size(); ++matchLen)
				if (cb[matchLen].c_!=magic[matchLen]) break;

			if (matchLen==magic.size())
				positions.push_back(cb[0].offset_);

			cb.pop_front();
		}

		for( ; ; ){
			// find likely gzipped trailer
			for( ; positions.size()>0; positions.pop_back()){
				iofs.seekg(positions.back());

				std::string content((std::istreambuf_iterator<char>(iofs) ),
									(std::istreambuf_iterator<char>()    ) );

				if (IsCompressedPaddingTrailer(iofs, positions.back()))
					break;
			}

			// couldn't find gzipped trailer
			if (positions.size()==0)
				throw std::logic_error("XmltarArchive::XmltarArchive: unable to identify trailer in xmltar file");

			// try to decompress up to this gzipped trailer
			iofs.seekg(0,std::ios_base::beg);

			Bidirectional_Pipe p;
			p.Open(
				CompressionCommand(options_.archiveMemberCompression_.get()),
				DecompressionArguments(options_.archiveMemberCompression_.get())
			);

			for(char c; iofs; iofs.get(c))
				if (iofs.tellg()==positions.back()) break;
				else {
					if (!p.ChildExitedAndAllPipesClosed() && p.Can_Write()){
						p.QueueWrite(c);
					}
					while(!p.ChildExitedAndAllPipesClosed()){
						if (p.Can_Read1()) p.Read1();
						if (p.Can_Read2()) p.Read2();
					}
				}

			p.QueueWriteClose();
			while(!p.ChildExitedAndAllPipesClosed()){
				if (p.Can_Read1()) p.Read1();
				if (p.Can_Read2()) p.Read2();
			}

			if (p.ExitStatus()==0) break;
		}

		iofs.seekg(positions.back());

		// we have found the trailer and the stream is positioned to overwrite the old trailer
	}
#endif
}

bool XmltarArchive::IsPaddingTrailer(std::string s){
	std::string openingPaddingTag="<padding>";
	std::string closingPaddingTag="</padding>";
	std::string closingArchiveTag="</xmltar>";

	std::string::size_type i=0;
	for( ; i<s.size(); ++i)
		if (!std::isspace(s[i])) break;

	if (i+openingPaddingTag.size()<=s.size() && s.substr(i,openingPaddingTag.size())==openingPaddingTag)
		i+=openingPaddingTag.size();
	else return false;

	for( ; i<s.size(); ++i)
		if (!std::isxdigit(s[i])) break;

	if (i+closingPaddingTag.size()<=s.size() && s.substr(i,closingPaddingTag.size())==closingPaddingTag)
		i+=closingPaddingTag.size();
	else return false;

	for( ; i<s.size(); ++i)
		if (!std::isspace(s[i])) break;

	if (i+closingArchiveTag.size()<=s.size() && s.substr(i,closingArchiveTag.size())==closingArchiveTag)
		i+=closingArchiveTag.size();
	else return false;

	if (i==s.size()) return true;
	else if (i==s.size()-1 && s[i]=='\n') return true;

	return false;
}

bool XmltarArchive::IsCompressedPaddingTrailer(std::fstream & iofs, std::ios::off_type offset){
	// We try to uncompress from file::offset to the end.
	// If it succeeds, and we match string s to the beginning, we are likely in the correct spot.

	iofs.seekg(offset);
	std::string compressedContent(	(std::istreambuf_iterator<char>(iofs)),
									(std::istreambuf_iterator<char>()    ));

	std::string uncompressedContent=options_.archiveCompression_.get()->OpenForceWriteAndClose(
										options_.archiveMemberCompression_.get()->OpenForceWriteAndClose(
											compressedContent
										)
									);

	return IsPaddingTrailer(uncompressedContent);
}

std::string XmltarArchive::ArchiveHeader(std::string filename, int archive_sequence_number){
    std::string s;

    s+="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"+options_.Newline();
    s+="<xmltar xmlns=\"http://www.xmltar.org/0.1\" version=\"0.1\">"+options_.Newline();
    s+=options_.Tabs("\t")+"<archive-name>"+filename+"</archive-name>"+options_.Newline();
    s+=options_.Tabs("\t")+"<members>"+options_.Newline();

    return s;
}

std::string XmltarArchive::CompressedArchiveHeader(std::string filename, int archive_sequence_number){
	return options_.archiveCompression_.get()->OpenForceWriteAndClose(
				options_.archiveMemberCompression_.get()->OpenForceWriteAndClose(
						ArchiveHeader(filename, archive_sequence_number)
				)
			);
}

std::string XmltarArchive::ArchiveTrailerBegin(){
    std::string s
		=options_.Tabs("\t")+"</members>"+options_.Newline()
		+options_.Tabs("\t")+"<padding>";

    return s;
}

std::string XmltarArchive::ArchiveTrailerMiddle(unsigned int padding){
	//std::string s=options_.archiveCompression_.get()->MinimumCompressionString();

    return std::string();
}

std::string XmltarArchive::ArchiveTrailerEnd(){
    std::string s="</padding>"+options_.Newline()+"</xmltar>"+options_.Newline();

    return s;
}

std::string XmltarArchive::CompressedArchiveTrailer(){

	std::string compressedArchiveTrailerBegin
			=options_.archiveCompression_.get()->OpenForceWriteAndClose(
				options_.archiveMemberCompression_.get()->OpenForceWriteAndClose(
								ArchiveTrailerBegin()
				)
			);

	std::string compressedArchiveTrailerEnd
			=options_.archiveCompression_.get()->OpenForceWriteAndClose(
				options_.archiveMemberCompression_.get()->OpenForceWriteAndClose(
								ArchiveTrailerEnd()
				)
			);

	std::string minimumString;

	std::string compressedArchiveTrailerMiddle
			=options_.archiveCompression_.get()->OpenForceWriteAndClose(
				options_.archiveMemberCompression_.get()->OpenForceWriteAndClose(
					minimumString
				)
			);

	return compressedArchiveTrailerBegin+compressedArchiveTrailerMiddle+compressedArchiveTrailerEnd;

	return "";
}

std::string XmltarArchive::CompressedArchiveTrailer(unsigned int desiredLength){
#if 0
	betz::Debug2 dbg("XmltarArchive::CompressedArchiveTrailer");

	std::cerr << dbg << ": archiveCompression=" << options_.archiveCompression_->Name() << std::endl;
	std::cerr << dbg << ": archiveMemberCompression=" << options_.archiveMemberCompression_->Name() << std::endl;
	std::cerr << dbg << ": desiredLength=" << desiredLength << std::endl;

	std::string compressedArchiveTrailerBegin
			=options_.archiveCompression_->OpenForceWriteAndClose(
				options_.archiveMemberCompression_->OpenForceWriteAndClose(
								ArchiveTrailerBegin()
				)
			);

	std::string compressedArchiveTrailerEnd
			=options_.archiveCompression_->OpenForceWriteAndClose(
				options_.archiveMemberCompression_->OpenForceWriteAndClose(
								ArchiveTrailerEnd()
				)
			);

	if (desiredLength<compressedArchiveTrailerBegin.size()+compressedArchiveTrailerEnd.size())
		throw std::logic_error("XmltarArchive::CompressedArchiveTrailer: desiredLength<compressedArchiveTrailerBegin.size()+compressedArchiveTrailerEnd.size()");

	desiredLength-=compressedArchiveTrailerBegin.size()+compressedArchiveTrailerEnd.size();

	std::cerr << dbg << ": 2 desiredLength=" << desiredLength << std::endl;

	std::string minimumString(
		random_hex+std::get<0>(dMap[options_.archiveCompression_->Name()][options_.archiveMemberCompression_->Name()].begin()->second),
		std::get<1>(dMap[options_.archiveCompression_->Name()][options_.archiveMemberCompression_->Name()].begin()->second));

	std::string minimumCompressedString
		=options_.archiveCompression_->OpenForceWriteAndClose(
				options_.archiveMemberCompression_->OpenForceWriteAndClose(
						minimumString
				)
			);

	std::string maximumString(
		random_hex+std::get<0>(dMap[options_.archiveCompression_->Name()][options_.archiveMemberCompression_->Name()].rbegin()->second),
		std::get<1>(dMap[options_.archiveCompression_->Name()][options_.archiveMemberCompression_->Name()].rbegin()->second));

	std::string maximumCompressedString
		=options_.archiveCompression_->OpenForceWriteAndClose(
				options_.archiveMemberCompression_->OpenForceWriteAndClose(
						maximumString
				)
			);

	if (desiredLength<minimumCompressedString.size())
		throw std::logic_error("XmltarArchive::CompressedArchiveTrailer: desiredLength<minimumCompressedString.size()");

	std::string compressedArchiveTrailerMiddle;

	std::cerr << dbg << ": 3 desiredLength=" << desiredLength << std::endl;
	std::cerr << dbg << ": minimumCompressedString.size()=" << minimumCompressedString.size() << std::endl;
	std::cerr << dbg << ": maximumCompressedString.size()=" << maximumCompressedString.size() << std::endl;

	for( ; desiredLength>maximumCompressedString.size(); desiredLength-=minimumCompressedString.size())
		compressedArchiveTrailerMiddle
			+=minimumCompressedString;

	if (desiredLength<minimumCompressedString.size())
		throw std::logic_error("XmltarArchive::CompressedArchiveTrailer: reduced desiredLength<minimumCompressedString.size()");


	std::string uncompressedPadding(
			random_hex+std::get<0>(dMap[options_.archiveCompression_->Name()][options_.archiveMemberCompression_->Name()][desiredLength]),
			std::get<1>(dMap[options_.archiveCompression_->Name()][options_.archiveMemberCompression_->Name()][desiredLength]));

	std::string compressedPadding
		=options_.archiveCompression_->OpenForceWriteAndClose(
			options_.archiveMemberCompression_->OpenForceWriteAndClose(
				uncompressedPadding
			)
		);

	std::cerr << "uncompressedPadding.size()=" << uncompressedPadding.size() << std::endl;
	std::cerr << "compressedPadding.size()=" << compressedPadding.size() << std::endl;

	compressedArchiveTrailerMiddle
			+=compressedPadding;

	return compressedArchiveTrailerBegin+compressedArchiveTrailerMiddle+compressedArchiveTrailerEnd;
#endif
	return "";
}
