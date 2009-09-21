/*
 * MetadataOnFlush.hpp
 *
 *  Created on: Sep 19, 2009
 *      Author: vjeko
 */

#ifndef METADATAONFLUSH_HPP_
#define METADATAONFLUSH_HPP_

#include <boost/noncopyable.hpp>

#include "../cache.hpp"

#include "../../accessor.hpp"
#include "../../xmlrpc/harmony.hpp"




template <typename T>
struct MetadataOnFlush : boost::noncopyable {

  MetadataOnFlush() {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> p) {
    DHT::Instance().set_pair(p);
  }

  void OnDone() {}
};


#endif /* METADATAONFLUSH_HPP_ */
