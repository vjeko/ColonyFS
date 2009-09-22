/*
 * util.cpp
 *
 *  Created on: Jul 14, 2008
 *      Author: vjeko
 */

#include "util.hpp"
#include "../debug.hpp"

#include <sstream>
#include <iostream>

#include <boost/foreach.hpp>

namespace colony { namespace storage {

/** GLobal Variables **/

/* Directory prefix that will be used for all I/O operations. */
boost::filesystem::path prefix;

/** End GLobal Variables **/

namespace bfs = boost::filesystem;

/* Private Functions & Variables */
namespace {

rlog::RLogChannel *storage_util_detail_(
    RLOG_CHANNEL( "storage/util/detail" ) );

}

void set_prefix(boost::filesystem::path path) {
  prefix = path;
}


void deposit_chunk(
    const boost::shared_ptr<chunk_data> chunk_ptr,
    db& database) {

  std::ostringstream representation;
  representation << prefix << chunk_ptr->uid_ << "-" << chunk_ptr->cuid_;

  boost::filesystem::ofstream stream(representation.str(),
      std::ios::out | std::ios::binary);

  stream.write(&chunk_ptr->data_ptr_->front(), chunk_ptr->data_ptr_->size());
  stream.close();

  database.save_chunk(*chunk_ptr, representation.str());
}

// TODO: User unbounded generics here, instead of inheritance.
boost::shared_ptr<chunk_data> retrieve_chunk(
    const boost::shared_ptr<basic_metadata> metadata,
    const db& database) {
  return retrieve_chunk(metadata->uid_, metadata->cuid_, database);
}

boost::shared_ptr<chunk_data> retrieve_chunk(
    const chunk_metadata::uid_type uid,
    const chunk_metadata::cuid_type cuid,
    const db& database) {

  boost::filesystem::path  read_path(database(uid)(cuid));
  int                      read_size = boost::filesystem::file_size(read_path);
  char                     _buffer[read_size];

  boost::filesystem::ifstream
    stream(read_path, std::ios::in | std::ios::binary);

  stream.read(_buffer, read_size);
  stream.close();

  boost::shared_ptr<chunk_data::data_type>
  data_ptr(new chunk_data::data_type(_buffer, _buffer + read_size) );

  return boost::shared_ptr<chunk_data>(new chunk_data(uid, cuid, data_ptr) );
}

} } // namespace
