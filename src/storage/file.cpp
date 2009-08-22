/*
 * file.cpp
 *
 *  Created on: Aug 5, 2008
 *      Author: vjeko
 */

#include "file.hpp"

namespace colony { namespace storage {

file::file() :
      uid_("UNSPECIFIED"),
      num_of_chunks_(0) {}

file::file(chunk_metadata::uid_type uid) :
      uid_(uid),
      num_of_chunks_(0) {}

file::~file() {}

void file::set_uid(chunk_metadata::uid_type uid) {
  uid_ = uid;
}

file::num_of_chunks_type file::get_num_of_chunks() const {
  return num_of_chunks_;
}

file::cuid_map_type::mapped_type& file::operator[](
    const chunk_metadata::cuid_type& cuid) {
  if (cuid + 1 > num_of_chunks_) num_of_chunks_ = cuid + 1;
  return cuid_map_[cuid];
}

const file::cuid_map_type::mapped_type& file::operator()(
    const chunk_metadata::cuid_type& cuid) const {

  cuid_map_type::const_iterator
    chunk_iterator = cuid_map_.find(cuid);

  if (chunk_iterator == cuid_map_.end())
    throw std::runtime_error("trying to index nonexistent chunk");

  return chunk_iterator->second;
}

} } // namespace
