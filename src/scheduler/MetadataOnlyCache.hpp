/*
 * MetadataOnlyCache.hpp
 *
 *  Created on: Sep 18, 2009
 *      Author: vjeko
 */

#ifndef METADATAONLYCACHE_HPP_
#define METADATAONLYCACHE_HPP_

#include <boost/noncopyable.hpp>

#include "../xmlrpc/harmony.hpp"




template <typename T>
struct MetadataOnlyCache : boost::noncopyable {

  MetadataOnlyCache() : accessor_("harmony-test") {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> p) {}

  void OnDone() {}

  colony::xmlrpc::harmony accessor_;

};

#endif /* METADATAONLYCACHE_HPP_ */
