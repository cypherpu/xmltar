/*
 * XmltarSocket.hpp
 *
 *  Created on: Jul 30, 2018
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARMEMBERSOCKET_HPP_
#define SRC_COMMON_XMLTAR_XMLTARMEMBERSOCKET_HPP_

#include "Xmltar/XmltarMember.hpp"
#include "Options/XmltarOptions.hpp"
#include <Transform/Transform.hpp>

extern "C" {
#include <sys/stat.h>
}

class XmltarMemberSocket : public XmltarMember {
public:
	XmltarMemberSocket(XmltarOptions const & options, boost::filesystem::path const & filepath);

	virtual void write(std::shared_ptr<Transform> archiveCompression, size_t committedBytes, size_t pendingBytes, std::ostream & ofs) override;

	virtual std::string MemberHeader() override;
	virtual std::string MemberTrailer() override;
	virtual bool CanArchive(size_t committedBytes, size_t pendingBytes, std::shared_ptr<Transform> archiveCompression) override;
};

#endif /* SRC_COMMON_XMLTAR_XMLTARMEMBERSOCKET_HPP_ */