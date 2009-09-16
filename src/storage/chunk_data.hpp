/*
 * chunk.h
 *
 *  Created on: Jul 18, 2008
 *      Author: vjeko
 */

#ifndef CHUNK_DATA_HPP_
#define CHUNK_DATA_HPP_

#include <boost/filesystem.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/scoped_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>

#include "chunk_metadata.hpp"

namespace colony {

namespace storage {

class chunk_data : public chunk_metadata {

public:

  typedef std::vector< char >          data_type;

  chunk_data();

  chunk_data(std::string               s);

  chunk_data(
      chunk_metadata::uid_type         uid,
      chunk_metadata::cuid_type        cuid);

  chunk_data(key_type key);

  chunk_data(
      chunk_metadata::uid_type         uid,
      chunk_metadata::cuid_type        cuid,
      boost::shared_ptr<data_type>     data_ptr);

  chunk_data(
      chunk_metadata                   metadata,
      boost::shared_ptr<data_type>     data_ptr);


	~chunk_data();

  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & boost::serialization::base_object<chunk_metadata>(*this);
    ar & data_ptr_;
  }

  boost::scoped_ptr<data_type>    data_ptr_;
};

}

}

#endif /* CHUNK_DATA_HPP_ */
