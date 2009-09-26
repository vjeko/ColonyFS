/*
 * util.cpp
 *
 *  Created on: Jul 14, 2008
 *      Author: vjeko
 */

#include "util.hpp"

#include "../debug.hpp"
#include "../exceptions.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>




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

  // Does the specified path exist?
  if ( !boost::filesystem::exists(path) ) {
    rError("Placement path does not exist: %s", path.string().c_str());
    throw colony::inexistent_file_e();
  }

  // Is the specified path a directory?
  if ( !boost::filesystem::is_directory(path) ) {
    rError("Placement path is not a directory: %s", path.string().c_str());
    throw colony::invalid_file_e();
  }

  // TODO: Is the specified path writable to?



  prefix = path;
}




void deposit_chunk(
    const boost::shared_ptr<chunk_data> chunk_ptr,
    db& database) {

  const std::string uid = chunk_ptr->uid_;
  const std::string cuid = boost::lexical_cast<std::string>(chunk_ptr->cuid_);


  boost::filesystem::path filepath = prefix / uid;
  boost::filesystem::path destionation("__" + filepath.filename() + "__" + cuid);

  // What if the file is not in the root directory?
  if( !boost::filesystem::exists(filepath.parent_path()) ) {
    boost::filesystem::create_directory(filepath.parent_path());
  }



  boost::filesystem::path representation = filepath.parent_path() / destionation;


  representation = representation.normalize();


  boost::filesystem::ofstream stream(representation.string(),
      std::ios::out | std::ios::binary);

  stream.write(&chunk_ptr->data_ptr_->front(), chunk_ptr->data_ptr_->size());
  stream.close();

  database.save_chunk(*chunk_ptr, representation.string());
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

  const std::string p = database(uid)(cuid);
  const boost::filesystem::path  read_path(p);

  ptrdiff_t                read_size = boost::filesystem::file_size(read_path);

  boost::filesystem::ifstream stream(read_path, std::ios::in | std::ios::binary);

  boost::shared_ptr<chunk_data> chunk_ptr = boost::make_shared<chunk_data>(uid, cuid);
  chunk_ptr->data_ptr_->resize(read_size);

  ptrdiff_t bytes_read = stream.readsome(&chunk_ptr->data_ptr_->front(), read_size);

  BOOST_ASSERT(bytes_read == read_size);

  stream.close();

  return chunk_ptr;
}

} } // namespace
