/*
 * util.cpp
 *
 *  Created on: Jul 14, 2008
 *      Author: vjeko
 */

#include "util.hpp"
#include "../debug.hpp"
#include "../hash/GeneralHashFunctions.h"

#include <sstream>
#include <iostream>

#include <boost/foreach.hpp>

namespace uledfs { namespace storage {

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

void async_assemble_chunks(
    const boost::filesystem::path& out,
    std::vector<boost::shared_ptr<uledfs::storage::chunk_data> > data_ptr_pool) {

  boost::filesystem::ofstream stream(out);

  for(size_t i = 0; i < data_ptr_pool.size(); i++) {
    stream.write(&data_ptr_pool[i]->data_ptr_->front(), data_ptr_pool[i]->data_ptr_->size());
  }

  stream.close();

  /*
   * TODO: Replace above statement with:
   *
   * BOOST_FOREACH(uledfs::storage::chunk_data& data, *data_pool_ptr) {
   *    stream.write(&data.data_ptr_->front(), data.data_ptr_->size());
   * }
   *
   * Make sure that there are no performance penalties.
   */

}

void async_assemble_chunks(
    const std::string& uid,
    const db& database,
    boost::filesystem::ofstream& stream) {

  int num_of_chunks = database.get_num_of_chunks(uid);
  rLog(storage_util_detail_,
      "%u chunks about to be assembled.", num_of_chunks);

  boost::shared_ptr<chunk_data> chunk_ptr;

  for (int cuid = 0; cuid < num_of_chunks; cuid++) {
    chunk_ptr = retrieve_chunk(uid, cuid, database);

    stream.write(&chunk_ptr->data_ptr_->front(), chunk_ptr->data_ptr_->size());

  }

}

boost::shared_ptr< std::vector< chunk_data > > chunk_file(
    const boost::filesystem::path& file_path) {

  /* File not found. Throw a run-time exception. */
  if (!boost::filesystem::exists(file_path)) {
    std::runtime_error("file does not exist");
  }

  bfs::ifstream input_file(file_path, std::ios::in | std::ios::binary);

  boost::shared_ptr< std::vector< chunk_data > >
    chunks_ptr(new std::vector< chunk_data >);

  if (input_file.is_open()) {

    /* Calculate the remaining file size (in bytes). */
    int remaining_size = boost::filesystem::file_size(file_path);

    rLog(storage_util_detail_, "Creating %d chunks.",
        (remaining_size + CHUNK_SIZE - 1)/CHUNK_SIZE);

    for (int cnt = 0; remaining_size > 0; cnt++) {

      int write_size;

      /*
       * Determine if we have enough data for the whole
       * chunk. If not, chunk of the correct size has to
       * be created.
       */
      if (remaining_size >= CHUNK_SIZE)
        write_size = CHUNK_SIZE;
      else
        write_size = remaining_size;

      char _buffer[write_size];
      input_file.read(_buffer, write_size);

      boost::shared_ptr<chunk_data::data_type>
        data_ptr(new chunk_data::data_type(_buffer, _buffer + write_size) );

      chunk_data chunk(file_path, cnt, data_ptr);

      chunks_ptr->push_back(chunk);
      remaining_size -= write_size;

    }

  } else {
    std::runtime_error("unable to open the file");
  }

  input_file.close();

  return chunks_ptr;

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
