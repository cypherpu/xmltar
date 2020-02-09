/*

Copyright 2010 by David A. Betz

This file is part of xmltar.

xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xmltar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef XMLTAROPTIONS_HPP_

#define XMLTAROPTIONS_HPP_

#include <limits>
#include <fstream>
#include <queue>

#include <boost/optional.hpp>

#include "Utilities/CompressRaw.hpp"
#include "Utilities/IdentityRaw.hpp"
#include "Utilities/Options-TarStyle.hpp"
#include "Compressors/Compressor.hpp"

class XmltarOptions {
public:
    enum Operation { NOOP, APPEND, CREATE, LIST, EXTRACT };
	enum Encoding { BASE16, BASE64 };

	boost::optional<Operation> operation_;
	boost::optional<int> verbosity_;
	boost::optional<bool> multi_volume_;

	std::shared_ptr<CompressorGeneralInterface> fileCompression_;
	std::shared_ptr<CompressorRawInterface> fileRawCompression_;
	std::shared_ptr<CompressorGeneralInterface> fileDecompression_;

	std::shared_ptr<CompressorGeneralInterface> encoding_;
	std::shared_ptr<CompressorGeneralInterface> decoding_;

	std::shared_ptr<CompressorGeneralInterface> archiveMemberCompression_;
	std::shared_ptr<CompressorRawInterface> archiveMemberRawCompression_;
	std::shared_ptr<CompressorGeneralInterface> archiveMemberDecompression_;

	std::shared_ptr<CompressorGeneralInterface> archiveCompression_;
	std::shared_ptr<CompressorRawInterface> archiveRawCompression_;
	std::shared_ptr<CompressorGeneralInterface> archiveDecompression_;

	boost::optional<size_t> tape_length_;
	boost::optional<size_t> stop_after_;
	boost::optional<std::filesystem::path> listed_incremental_file_;
	std::shared_ptr<CompressorGeneralInterface> incrementalFileCompression_;
	std::shared_ptr<CompressorGeneralInterface> incrementalFileDecompression_;
	boost::optional<unsigned int> dump_level_;
	boost::optional<std::filesystem::path> files_from_;
	std::vector<std::string> excludeFileGlobs_;
	boost::optional<std::string> archiveMemberTag_;
	boost::optional<bool> tabs_;
	boost::optional<bool> newlines_;
	boost::optional<std::string> base_xmltar_file_name_;		// xmltar_file;
	boost::optional<unsigned int> starting_volume_;				// starting_volume;
	std::vector<std::string> sourceFileGlobs_;

    XmltarOptions(void)
        : operation_(), verbosity_(), multi_volume_(),
		  fileCompression_(new Compressor<Identity>()),
		  fileRawCompression_(new CompressorRaw<IdentityRaw>()),
		  encoding_(new Compressor<HexEncode>()),
		  archiveMemberCompression_(new Compressor<Identity>()),
		  archiveMemberRawCompression_(new CompressorRaw<IdentityRaw>()),
		  archiveCompression_(new Compressor<Identity>()),
		  archiveRawCompression_(new CompressorRaw<IdentityRaw>()),
          tape_length_(), stop_after_(std::numeric_limits<size_t>::max()),
		  listed_incremental_file_(),
		  incrementalFileCompression_(new Compressor<Identity>()),
		  dump_level_(),
		  files_from_(), excludeFileGlobs_(),
		  archiveMemberTag_(),tabs_(), newlines_(),
		  base_xmltar_file_name_(), starting_volume_(), sourceFileGlobs_(){ }

    void ProcessOptions(int argc, char const *argv[]);

	std::string Tabs(char const *tabStr) const {
		if (tabs_)
			return tabs_.get()?std::string(tabStr):std::string("");
		else return std::string(tabStr);
	}

	std::string Newline() const {
		if (newlines_)
			return newlines_.get()?std::string("\n"):std::string("");
		else return std::string("\n");
	}

	std::string toXMLString();
};

#endif // XMLTAROPTIONS_HPP_
