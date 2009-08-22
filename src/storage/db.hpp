/*
 * db.hpp
 *
 *  Created on: Jul 31, 2008
 *      Author: vjeko
 */

#ifndef DB_HPP_
#define DB_HPP_

#include "chunk_metadata.hpp"
#include "file.hpp"

#include <map>
#include <string>

namespace colony { namespace storage {

class db {
public:
  typedef std::string representation_type;

  typedef std::map<
    chunk_metadata::uid_type,
    file
  > uid_map_type;

  db();
  ~db();

  const file& operator()(const chunk_metadata::uid_type uid) const;
  file& operator[](const chunk_metadata::uid_type uid);

  void save_chunk(
      const chunk_metadata& metadata,
      const representation_type& representation);

  representation_type load_chunk(
      const chunk_metadata::uid_type uid,
      const chunk_metadata::cuid_type cuid) const;

  representation_type load_chunk(const chunk_metadata metadata) const;

  file::num_of_chunks_type get_num_of_chunks(
      const chunk_metadata::uid_type& uid) const;

private:

  uid_map_type   uid_map_;
  file           file_;

};

} } //namespace

#endif /* DB_HPP_ */
