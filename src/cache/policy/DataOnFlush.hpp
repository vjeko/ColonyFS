/*
 * DataOnFlush.hpp
 *
 *  Created on: Sep 19, 2009
 *      Author: vjeko
 */

#ifndef DATAONFLUSH_HPP_
#define DATAONFLUSH_HPP_

#include <boost/noncopyable.hpp>

#include "MetadataOnFlush.hpp"

#include "../cache.hpp"
#include "../../accessor.hpp"
#include "../../algo/CSScheduler.hpp"




template <typename T>
struct DataOnFlush : boost::noncopyable {

  typedef colony::xmlrpc::chunk_value C;

  DataOnFlush() {}

  void NoOp(T* p) {}

  void PreRead(shared_ptr<T> p) {

    const std::string hostname = colony::CSScheduler::GetCS();

    Client::Instance().retrieve_chunk(hostname, p);

    Client::IoService().reset();
    Client::IoService().run();
  }

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnErase(shared_ptr<T> p) {}

  void OnFlush(shared_ptr<T> p) {

    const std::string hostname = colony::CSScheduler::GetCS();

    Client::Instance().deposit_chunk(hostname, p);

    std::string key(boost::lexical_cast<std::string>(p->cuid_) + p->uid_);

    shared_ptr<C> tmp(new C(key));
    shared_ptr<C> chunk_info = cache_[tmp->get_key()];
    chunk_info->set_mapped(hostname);
  }

  void OnDone() {
    cache_.flush();

    Client::IoService().reset();
    Client::IoService().run();
  }


  colony::cache<C, MetadataOnFlush<C> > cache_;
};

#endif /* DATAONFLUSH_HPP_ */
