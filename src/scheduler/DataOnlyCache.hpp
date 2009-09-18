/*
 * MetadataOnlyCache.hpp
 *
 *  Created on: Sep 18, 2009
 *      Author: vjeko
 */

#ifndef DATAONLYCACHE_HPP_
#define DATAONLYCACHE_HPP_

#include "MetadataAccessor.hpp"
#include "../intercom/user_harmony.hpp"
#include "../storage/cache.hpp"

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>




template <typename T>
struct DataOnlyCache : boost::noncopyable {

  typedef colony::xmlrpc::chunk_value C;

  DataOnlyCache() : accessor_(io_service_, "harmony-test") {}

  void NoOp(T* p) {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> value) {}

  void OnDone() {}


  colony::cache<C, MetadataDeleter<C> >  cache_;
  boost::asio::io_service                io_service_;
  colony::intercom::user_harmony         accessor_;
};
#endif /* METADATAONLYCACHE_HPP_ */
