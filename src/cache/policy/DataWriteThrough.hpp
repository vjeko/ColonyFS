/*
 * DataWriteThrough.hpp
 *
 *  Created on: Sep 19, 2009
 *      Author: vjeko
 */

#ifndef DATAWRITETHROUGH_HPP_
#define DATAWRITETHROUGH_HPP_


#include "MetadataOnFlush.hpp"
#include "../../intercom/user_harmony.hpp"
#include "../cache.hpp"

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>




template <typename T>
struct DataWriteThrough : boost::noncopyable {

  typedef colony::xmlrpc::chunk_value C;

  DataWriteThrough() : accessor_(io_service_, "harmony-test") {}

  void NoOp(T* p) {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> value) {
    accessor_.deposit_chunk("codered", value);

    io_service_.reset();
    io_service_.run();

    std::string key(boost::lexical_cast<std::string>(value->cuid_) + value->uid_);

    shared_ptr<C> chunk_info = cache_[key];
    chunk_info->set_mapped("codered");
  }

  void OnDone() {
    cache_.flush();
  }


  colony::cache<C, MetadataOnFlush<C> > cache_;
  boost::asio::io_service io_service_;
  colony::intercom::user_harmony accessor_;
};

#endif /* DATAWRITETHROUGH_HPP_ */
