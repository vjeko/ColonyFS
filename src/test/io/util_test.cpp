/*
 * util_test.cpp
 *
 *  Created on: Jul 16, 2008
 *      Author: vjeko
 */

#include "../../storage/util.hpp"
#include "../../storage/db.hpp"
#include "../../debug.hpp"
#include "../../hash/GeneralHashFunctions.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

int main(int argc, char* argv[]) {

  rlog::RLogInit( argc, argv );
  rlog::StdioNode stdLog;

  stdLog.subscribeTo( RLOG_CHANNEL("debug") );
  stdLog.subscribeTo( RLOG_CHANNEL("storage/util/detail") );

  boost::shared_ptr< std::vector< colony::storage::chunk_data > > chunk_nptr;

  if (argc < 2)
    throw std::runtime_error("unspecified file");

  boost::filesystem::path sample_file(argv[1]);
  chunk_nptr = colony::storage::chunk_file(sample_file);

  rDebug("Created %d chunks.", (int) chunk_nptr->size());
  colony::storage::db db;

  BOOST_FOREACH(colony::storage::chunk_data& chunk, *chunk_nptr) {

    std::ostringstream file_name;
    file_name << chunk.uid_ << "-" << chunk.cuid_;

    db.save_chunk(chunk, file_name.str());

    rDebug("RS-Hash Function Value: %u",
    JSHash( std::string( &chunk.data_ptr_->front() ) ) );

   colony::storage::deposit_chunk(chunk, file_name.str());
  }

  colony::storage::db::representation_type file_name;

  BOOST_FOREACH(colony::storage::chunk_metadata& metadata, *chunk_nptr) {

    file_name = metadata.uid_;

    boost::shared_ptr<colony::storage::chunk_data> chunk =
      colony::storage::retrieve_chunk(metadata, db);

    rDebug("RS-Hash Function Value: %u",
    JSHash( std::string( &chunk->data_ptr_->front() ) ) );

  }
  boost::filesystem::path write_path("WRITE");
  boost::filesystem::ofstream
  stream(write_path, std::ios::out | std::ios::binary);;

  colony::storage::async_assemble_chunks(file_name, db, stream);

}
