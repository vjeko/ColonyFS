/*
 * file.hpp
 *
 *  Created on: Aug 5, 2008
 *      Author: vjeko
 */

#ifndef FILE_HPP_
#define FILE_HPP_

#include "chunk_metadata.hpp"

#include <map>

namespace colony { namespace storage {

class file {

public:

  typedef int32_t         num_of_chunks_type;
  typedef std::string     representation_type;
  typedef std::map<
    chunk_metadata::cuid_type,
    representation_type
  > cuid_map_type;

  file();
  file(chunk_metadata::uid_type uid);
  virtual ~file();

  void set_uid(chunk_metadata::uid_type uid);

  num_of_chunks_type get_num_of_chunks() const;

  cuid_map_type::mapped_type& operator[](
      const chunk_metadata::cuid_type& cuid);

  const cuid_map_type::mapped_type& operator()(
      const chunk_metadata::cuid_type& cuid) const;

private:

  chunk_metadata::uid_type  uid_;
  cuid_map_type             cuid_map_;
  num_of_chunks_type        num_of_chunks_;

};

} } // namespace

#endif /* FILE_HPP_ */
