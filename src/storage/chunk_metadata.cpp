/*
 * chunk.cpp
 *
 *  Created on: Jul 15, 2008
 *      Author: vjeko
 */

#include "chunk_metadata.hpp"

namespace uledfs {

namespace storage {

chunk_metadata::chunk_metadata() :
    basic_metadata::basic_metadata("NONE", 1) {}

chunk_metadata::chunk_metadata(cuid_type cuid) :
    basic_metadata::basic_metadata("UNSPECIFIED" ,cuid) {}

chunk_metadata::chunk_metadata(
    uid_type uid,
    cuid_type cuid) :
      basic_metadata::basic_metadata(uid, cuid) {}

chunk_metadata::chunk_metadata(
    boost::filesystem::path path,
    cuid_type cuid) :
      basic_metadata::basic_metadata(path.leaf(), cuid) {}

chunk_metadata::~chunk_metadata() {}

}

}
