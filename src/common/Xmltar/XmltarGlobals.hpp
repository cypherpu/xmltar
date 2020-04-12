/*
 * XmltarGlobals.hpp
 *
 *  Created on: Apr 27, 2019
 *      Author: dbetz
 */

#ifndef SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_
#define SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_

#include <string>
#include <memory>
#include <queue>
#include <filesystem>

#include <fnmatch.h>
#include <sys/random.h>
#include <openssl/evp.h>

#include "Utilities/ExtendedPath.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Xmltar/XmltarOptions.hpp"

class XmltarMemberCreate;

class XmltarGlobals {
public:
	static const size_t xChaCha20Poly1305MessageLength=1<<15;

	XmltarOptions options_;
	std::string current_xmltar_file_name_;
	size_t current_volume_;
	std::unique_ptr<Snapshot> snapshot_;	// FIXME - make sure snapshot_ is destructed before options
	time_t invocationTime_;

	std::priority_queue<ExtendedPath,std::vector<ExtendedPath>,std::greater<ExtendedPath>> filesToBeIncluded_;
	std::priority_queue<ExtendedPath,std::vector<ExtendedPath>,std::greater<ExtendedPath>> filesToBeExcluded_;

	std::string Sha3(std::filesystem::path filepath);
	std::string nextAction_;
	std::unique_ptr<XmltarMemberCreate> nextMember_;

	std::string passphrase_;
	std::string key_;

    int resultCode_;
    std::vector<std::string> errorMessages_;

    XmltarGlobals();
    void NextMember();
    bool MatchesGlobs(std::filesystem::path p, std::vector<std::string> globs){
    	for(auto & s : globs){
    		int result=fnmatch(p.string().c_str(),s.c_str(),FNM_PATHNAME|FNM_PERIOD);
    		if (result==0) return true;
    		else if (result!=FNM_NOMATCH)
    			throw std::runtime_error("XmltarGlobals::MatchesGlobs: unknown error");
    	}

    	return false;
    }

    bool IncludedFile(ExtendedPath p){
    	return MatchesGlobs(p.path(),options_.sourceFileGlobs_);
    }

    bool ExcludedFile(ExtendedPath p){
    	return MatchesGlobs(p.path(),options_.excludeFileGlobs_);
    }

    void AddSubdirectories(std::filesystem::path const & p){
		std::filesystem::file_status f_stat=std::filesystem::symlink_status(p);

		if (std::filesystem::is_directory(f_stat)){
			for(auto & i : std::filesystem::directory_iterator(p) ){
				filesToBeIncluded_.push(ExtendedPath(i));
			}
		}
    }

    std::string InitializationVector(int nBytes){
    	std::string result(nBytes,' ');

    	if (getrandom(result.data(),nBytes,GRND_RANDOM)!=nBytes)
    		throw std::runtime_error("XmltarGlobals::InitializationVector: unable to getrandom");

    	return result;
    }

    std::string KeyFromPassphrase(std::string const & passphrase){
    	std::string salt=
    			"\xff\x29\x2c\x48\x44\x4d\xb2\x7b\x4b\xfc\x24\x69\xd9\x92\xc0\x10"
    			"\x3d\xf7\x46\xa1\x1c\x7b\x3d\xf7\x4f\xba\xc0\x0c\x94\xf7\xad\xec"
    			"\xff\x0f\xd9\xaa\x0e\x8e\x5b\x55\xd7\xe2\x97\xc0\x39\x69\xa8\x5a"
    			"\xb5\x08x\b7\xdb\x1a\xbb\xe8\x8b\x41\xf2\xe7\xac\x47\x20\xba\x36";
    	std::string key(64,' ');

		if (PKCS5_PBKDF2_HMAC(
				passphrase.data(), passphrase.size(),
				reinterpret_cast<unsigned const char *>(salt.data()), salt.size(),
				1000, EVP_sha3_512(), key.size(), reinterpret_cast<unsigned char *>(key.data())
			)!=1)
    		throw std::runtime_error("XmltarGlobals::InitializationVector: unable to getrandom");

		return key;
    }
};



#endif /* SRC_COMMON_XMLTAR_XMLTARGLOBALS_HPP_ */
