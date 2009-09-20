/*
 * MetadataWriteThrough.hpp
 *
 *  Created on: Sep 19, 2009
 *      Author: vjeko
 */

#ifndef METADATAWRITETHROUGH_HPP_
#define METADATAWRITETHROUGH_HPP_

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

#include "../../xmlrpc/harmony.hpp"




template <typename T>
struct MetadataWriteThrough : boost::noncopyable {

  MetadataWriteThrough() : accessor_("harmony-test") {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {
    boost::shared_ptr<T> value(p, boost::bind(&MetadataWriteThrough::NoOp, this, _1));
    accessor_.set_pair(value);
  }

  void OnFlush(shared_ptr<T> p) {}

  void OnDone() {}

  void NoOp(T* p) {}

  colony::xmlrpc::harmony accessor_;

};

#endif /* METADATAWRITETHROUGH_HPP_ */
