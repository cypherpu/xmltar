/*
 * XmltarArchiveExtractSingleVolume.cpp
 *
 *  Created on: Mar 16, 2019
 *      Author: dbetz
 */

#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "Xmltar/XmltarArchiveExtractSingleVolume.hpp"

void XmltarSingleVolumeXmlHandler::startElement(const XML_Char *name, const XML_Char **atts){
	if (elements_.back().name_=="xmltar"){
		if (elements_.size()!=1) throw std::domain_error("XmltarArchiveHandler::startElement: \"xmltar\" wrong nesting level");
	}
	else if (elements_.back().name_=="members"){
		if (elements_.size()!=2) throw std::domain_error("XmltarArchiveHandler::startElement: \"members\" wrong nesting level");
	}
	else if (elements_.back().name_=="file"){
		if (elements_.size()!=3) throw std::domain_error("XmltarArchiveHandler::startElement: \"file\" wrong nesting level");
		else if (elements_.end()[-2].name_!="members") throw std::domain_error("XmltarArchiveHandler::startElement \"members\" not parent of \"file\"");
	}
	else if (elements_.back().name_=="content"){
		if (elements_.size()!=4) throw std::domain_error("XmltarArchiveHandler::startElement: \"content\" wrong nesting level");
		else if (elements_.end()[-2].name_!="file") throw std::domain_error("XmltarArchiveHandler::startElement \"file\" not parent of \"content\"");
	}
	else if (elements_.back().name_=="stream"){
		if (elements_.size()!=5) throw std::domain_error("XmltarArchiveHandler::startElement: \"stream\" wrong nesting level");
		else if (elements_.end()[-2].name_!="content") throw std::domain_error("XmltarArchiveHandler::startElement \"content\" not parent of \"stream\"");
	}

	if (elements_.back().name_=="content" && elements_.back().attributes_.at("type")=="directory")
			std::filesystem::create_directories(elements_.end()[-2].attributes_.at("name"));
	else if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		// create directories leading to file
		//std::cerr << "XmltarMultiVolumeXmlHandler::startElement: before create directories" << std::endl;
		std::filesystem::path p=std::filesystem::path(elements_.end()[-3].attributes_.at("name"));
		if (p.has_parent_path())
			std::filesystem::create_directories(p.parent_path());
		// FIXME - investigate file open flags
		xmltarArchiveExtractSingleVolume_.fs_.open(elements_.end()[-3].attributes_.at("name"),std::fstream::app);
		std::cerr << std::string(elements_.size(),'\t') << "boost::lexical_cast<std::streamoff>(elements_.back().attributes_.at(\"this-extent-start\"))=" << boost::lexical_cast<std::streamoff>(elements_.back().attributes_.at("this-extent-start")) << std::endl;
		xmltarArchiveExtractSingleVolume_.fs_.seekp(boost::lexical_cast<std::streamoff>(elements_.back().attributes_.at("this-extent-start")),std::ios_base::beg);

		xmltarArchiveExtractSingleVolume_.decoder_.reset(xmltarArchiveExtractSingleVolume_.options_.encoding_->clone());
		xmltarArchiveExtractSingleVolume_.decoder_->Open();

		xmltarArchiveExtractSingleVolume_.fileDecompression_.reset(xmltarArchiveExtractSingleVolume_.options_.fileCompression_->clone());
		xmltarArchiveExtractSingleVolume_.fileDecompression_->Open();
	}

	std::cerr << std::string(elements_.size(),'\t') << "<" << name << ">" << std::endl;
}

void XmltarSingleVolumeXmlHandler::endElement(const XML_Char *name){
	std::cerr << std::string(elements_.size(),'\t') << "</" << name << ">" << std::endl;

	if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		xmltarArchiveExtractSingleVolume_.fs_ << xmltarArchiveExtractSingleVolume_.fileDecompression_->ForceWriteAndClose(xmltarArchiveExtractSingleVolume_.decoder_->ForceWriteAndClose(""));
		xmltarArchiveExtractSingleVolume_.fs_.close();
	}

}

void XmltarSingleVolumeXmlHandler::characterData(XML_Char const *s, int len){
	if (elements_.back().name_=="stream" && elements_.end()[-2].attributes_.at("type")=="regular"){
		xmltarArchiveExtractSingleVolume_.fs_ << xmltarArchiveExtractSingleVolume_.fileDecompression_->ForceWrite(xmltarArchiveExtractSingleVolume_.decoder_->ForceWrite(std::string(s,len)));
	}
}

XmltarArchiveExtractSingleVolume::XmltarArchiveExtractSingleVolume(XmltarOptions const & opts, XmltarGlobals & globals, std::string filename, std::shared_ptr<XmltarMemberCreate> & nextMember)
	: ::XmltarArchive(opts,globals,filename,nextMember_)
{
	std::cerr << "XmltarArchiveExtractMultiVolume::XmltarSingleVolumeXmlHandler: entering: filename=" << filename << std::endl;


	XmltarSingleVolumeXmlHandler xmltarSingleVolumeHandler(*this);

	std::ifstream ifs(filename);

	XML_Char buffer[1024];

	std::shared_ptr<CompressorInterface> archiveDecompression(opts.archiveCompression_->clone());
	std::shared_ptr<CompressorInterface> memberDecompression(opts.archiveMemberCompression_->clone());

	archiveDecompression->Open();
	memberDecompression->Open();

	std::string tmp;
	while(ifs){
		ifs.read(buffer,sizeof(buffer)/sizeof(*buffer));

		tmp=memberDecompression->ForceWrite(archiveDecompression->ForceWrite(std::string(buffer,ifs.gcount())));
		//std::cerr << "ifs.gcount()=" << ifs.gcount() << std::endl;
		xmltarSingleVolumeHandler.Parse(tmp,false);
	}

	tmp=memberDecompression->ForceWriteAndClose(archiveDecompression->ForceWriteAndClose(""));
	//std::cerr << "ifs.gcount()=" << ifs.gcount() << std::endl;
	xmltarSingleVolumeHandler.Parse(tmp,false);
}


