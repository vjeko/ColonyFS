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

#include <sstream>




template <typename T>
struct MetadataOnFlush : boost::noncopyable {

  MetadataOnFlush() {}

  shared_ptr<T> PreRead(typename T::key_type key) {

    shared_ptr<T> p(new T);
    p->set_key(key);


    try {

      DHT::Instance().get_pair(p);
      rInfo("Value found!");
      return p;

    } catch (colony::xmlrpc::key_missing_error& e) {
      throw colony::lookup_e();
    }
  }

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> p) {
    DHT::Instance().set_pair(p);
  }

  void OnDone() {}
};


#endif /* METADATAONFLUSH_HPP_ */
