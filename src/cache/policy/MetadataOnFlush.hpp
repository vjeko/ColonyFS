/*
 * MetadataOnFlush.hpp
 *
 *  Created on: Sep 19, 2009
 *      Author: vjeko
 */

#ifndef METADATAONFLUSH_HPP_
#define METADATAONFLUSH_HPP_

#include <boost/noncopyable.hpp>

#include "../../xmlrpc/harmony.hpp"




template <typename T>
struct MetadataOnFlush : boost::noncopyable {

  MetadataOnFlush() : accessor_("harmony-test") {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> p) {
    accessor_.set_pair(p);
  }

  void OnDone() {}

  colony::xmlrpc::harmony accessor_;

};


#endif /* METADATAONFLUSH_HPP_ */
