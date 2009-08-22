/*
 * db.cpp
 *
 *  Created on: Jul 31, 2008
 *      Author: vjeko
 */

#include "db.hpp"

#include <iostream>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include <utility>

namespace colony { namespace storage {

db::db() :
      file_(file("UNSPECIFIED")) {}

db::~db() {}

const file& db::operator()(const chunk_metadata::uid_type uid) const {

  uid_map_type::const_iterator uid_iterator = uid_map_.find(uid);

  if (uid_iterator == uid_map_.end()) {
    throw std::runtime_error("trying to index inexistent file");
  }

  return uid_iterator->second;
}

file& db::operator[](const chunk_metadata::uid_type uid) {
  return uid_map_[uid];
}

void db::save_chunk(
    const chunk_metadata& metadata,
    const db::representation_type& representation) {

  (*this)[metadata.uid_][metadata.cuid_] = representation;
}

db::representation_type db::load_chunk(
    const chunk_metadata::uid_type uid,
    const chunk_metadata::cuid_type cuid) const {
  return (*this)(uid)(cuid);
}

db::representation_type db::load_chunk(const chunk_metadata metadata) const {
  return (*this)(metadata.uid_)(metadata.cuid_);
}

file::num_of_chunks_type db::get_num_of_chunks(
    const chunk_metadata::uid_type& uid) const {
  return (*this)(uid).get_num_of_chunks();
}

} } // namespace
