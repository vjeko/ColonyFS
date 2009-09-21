/*
 * DataOnFlush.hpp
 *
 *  Created on: Sep 19, 2009
 *      Author: vjeko
 */

#ifndef DATAONFLUSH_HPP_
#define DATAONFLUSH_HPP_

#include "MetadataOnFlush.hpp"
#include "../../intercom/user_harmony.hpp"
#include "../../accessor.hpp"

#include "../cache.hpp"

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/pool/singleton_pool.hpp>




template <typename T>
struct DataOnFlush : boost::noncopyable {

  typedef colony::xmlrpc::chunk_value C;

  DataOnFlush() {}

  void NoOp(T* p) {}

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> value) {
    Client::Instance().deposit_chunk("codered", value);

    std::string key(boost::lexical_cast<std::string>(value->cuid_) + value->uid_);

    shared_ptr<C> chunk_info = cache_[key];
    chunk_info->set_mapped("codered");
  }

  void OnDone() {
    cache_.flush();

    Client::IoService().reset();
    Client::IoService().run();
  }


  colony::cache<C, MetadataOnFlush<C> > cache_;
};

#endif /* DATAONFLUSH_HPP_ */
