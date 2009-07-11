/*
 * network_metadata.h
 *
 *  Created on: Aug 26, 2008
 *      Author: vjeko
 */

#ifndef NETWORK_METADATA_HPP_
#define NETWORK_METADATA_HPP_

#include "basic_metadata.hpp"

#include <string>

#include <boost/serialization/base_object.hpp>

namespace uledfs { namespace storage {

struct network_metadata : basic_metadata {

  typedef std::string host_type;

  network_metadata(uid_type uid, cuid_type cuid, host_type host) :
    basic_metadata(uid, cuid), host_(host) {}

  network_metadata(basic_metadata basic, host_type host) :
    basic_metadata(basic), host_(host) {}

  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version) {

    ar & boost::serialization::base_object<basic_metadata>(*this);
    ar & host_;

  }

  host_type   host_;

  void print() const {
    printf("%8s | %4d | %8s\n", uid_.c_str(), cuid_, host_.c_str() );
  }

};

} } // Namespace

#endif /* NETWORK_METADATA_HPP_ */
