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

  void PreRead(shared_ptr<T> p) {
    try {

      DHT::Instance().get_pair(p);
      rInfo("Value found!");

    } catch (colony::xmlrpc::key_missing_error& e) {
      throw colony::lookup_e();
    }
  }

  void OnRead(T* p) {}




  void OnWrite(T* p) {}




  void OnErase(shared_ptr<T> p) {

    const std::string delete_value("");
    DHT::Instance().put(p->get_key(), delete_value);
  }




  void OnFlush(shared_ptr<T> p) {}

  void OnDone() {}
};


#endif /* METADATAONFLUSH_HPP_ */
