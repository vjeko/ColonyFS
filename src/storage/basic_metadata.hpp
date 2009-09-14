/*
 * basic_metadata.hpp
 *
 *  Created on: Aug 26, 2008
 *      Author: vjeko
 */

#ifndef BASIC_METADATA_HPP_
#define BASIC_METADATA_HPP_

#include <string>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

namespace colony { namespace storage {

struct basic_metadata {

  typedef std::string                                      uid_type;
  typedef int                                              cuid_type;
  typedef boost::tuple<const uid_type, const cuid_type>    key_type;

  basic_metadata(uid_type uid, cuid_type cuid) :
    uid_(uid), cuid_(cuid) {}

  basic_metadata(key_type key) :
    uid_(key.get<0>()), cuid_(key.get<1>()) {}

  virtual ~basic_metadata() {}

  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version) {

    ar & uid_;
    ar & cuid_;

  }

  uid_type   uid_;
  cuid_type  cuid_;
};

} } // Namespace

#endif /* BASIC_METADATA_HPP_ */
