/*
 * DataAccessor.hpp
 *
 *  Created on: Sep 17, 2009
 *      Author: vjeko
 */

#ifndef DATAACCESSOR_HPP_
#define DATAACCESSOR_HPP_

#include "MetadataAccessor.hpp"
#include "../intercom/user_harmony.hpp"
#include "../storage/cache.hpp"

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>




template <typename T>
struct DataDeleter : boost::noncopyable {

  typedef colony::xmlrpc::chunk_value C;

  DataDeleter() : accessor_(io_service_, "harmony-test") {}

  void NoOp(T* p) {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> value) {
    accessor_.deposit_chunk("codered", value);
    // TODO: Commit chunk metadata.
  }

  void OnDone() {
    cache_.flush();

    io_service_.reset();
    io_service_.run();
  }


  colony::cache<C, MetadataDeleter<C> >  cache_;
  boost::asio::io_service                io_service_;
  colony::intercom::user_harmony         accessor_;
};

#endif /* DATAACCESSOR_HPP_ */
