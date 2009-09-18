/*
 * MetadataAccessor.hpp
 *
 *  Created on: Sep 17, 2009
 *      Author: vjeko
 */

#ifndef METADATAACCESSOR_HPP_
#define METADATAACCESSOR_HPP_

#include <boost/noncopyable.hpp>

#include "../xmlrpc/harmony.hpp"




template <typename T>
struct MetadataDeleter : boost::noncopyable {

  MetadataDeleter() : accessor_("harmony-test") {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> p) {
    accessor_.set_pair(p);
  }

  void OnDone() {}

  colony::xmlrpc::harmony accessor_;

};

#endif /* METADATAACCESSOR_HPP_ */
