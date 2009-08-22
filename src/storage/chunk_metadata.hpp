/*
 * chunk.hpp
 *
 *  Created on: Jul 15, 2008
 *      Author: vjeko
 */

#ifndef CHUNK_HPP_
#define CHUNK_HPP_

#include "storage.hpp"
#include "basic_metadata.hpp"

#include <string>

#include <boost/array.hpp>
#include <boost/filesystem.hpp>

#include <boost/serialization/base_object.hpp>

const int CHUNK_SIZE = 1024*1024*10;
typedef boost::array< char, CHUNK_SIZE > chunk_t;

namespace colony { namespace storage {

class chunk_metadata : public basic_metadata {

public:

  chunk_metadata();
  chunk_metadata(cuid_type);
  chunk_metadata(uid_type, cuid_type);
  chunk_metadata(boost::filesystem::path, cuid_type);

  virtual ~chunk_metadata();

  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & boost::serialization::base_object<basic_metadata>(*this);
  }

  bool operator>(const chunk_metadata& other) const {
    return this->uid_ > other.uid_;
  }

  bool operator<(const chunk_metadata& other) const {
    return this->uid_ < other.uid_;
  }

  bool operator==(const chunk_metadata& other) const {
    return this->uid_ == other.uid_;
  }

};


} }
#endif /* CHUNK_HPP_ */
