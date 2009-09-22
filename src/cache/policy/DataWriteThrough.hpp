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

  DataWriteThrough() {}

  void NoOp(T* p) {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> value) {

    const std::string hostname("codered");

    Client::Intance().deposit_chunk(hostname, value);

    Client::IoService().reset();
    Client::IoService().run();

    std::string key(boost::lexical_cast<std::string>(value->cuid_) + value->uid_);

    shared_ptr<C> chunk_info = cache_[key];
    chunk_info->set_mapped(hostname);
  }

  void OnDone() {
    cache_.flush();
  }


  colony::cache<C, MetadataOnFlush<C> > cache_;
};

#endif /* DATAWRITETHROUGH_HPP_ */
