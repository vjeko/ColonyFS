/*
 * cutil.hpp
 *
 *  Created on: Jul 14, 2008
 *      Author: vjeko
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include "chunk_data.hpp"
#include "db.hpp"

#include <vector>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/serialization/utility.hpp>

namespace colony { namespace storage {

  void set_prefix(boost::filesystem::path path);

  boost::shared_ptr< std::vector< chunk_data > > chunk_file(
      const boost::filesystem::path& file_path);

  void deposit_chunk(
      const boost::shared_ptr<chunk_data> chunk,
      db& database);

  boost::shared_ptr<chunk_data> retrieve_chunk(
      const boost::shared_ptr<basic_metadata> metadata,
      const db& database);

  boost::shared_ptr<chunk_data> retrieve_chunk(
      const chunk_metadata::uid_type uid,
      const chunk_metadata::cuid_type cuid,
      const db& database);

  void async_assemble_chunks(
      const std::string& uid,
      const db& database,
      boost::filesystem::ofstream& stream);

  void async_assemble_chunks(
      const boost::filesystem::path& out,
      std::vector<boost::shared_ptr<colony::storage::chunk_data> > data_ptr_pool) ;

} } // namespace

#endif /* UTIL_HPP_ */
