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




template <typename T>
struct DataOnFlush : boost::noncopyable {

  typedef colony::xmlrpc::chunk_value C;

  DataOnFlush() {}

  void NoOp(T* p) {}

  shared_ptr<T> PreRead(typename T::key_type key) {
    shared_ptr<T> p(new T(key));
    std::string hostname("codered");

    Client::Instance().retrieve_chunk(hostname, p);

    rDebug("about to run io service...");

    Client::IoService().reset();
    Client::IoService().run();

    rDebug("... done running io service.");

    rDebug("Chunk size: %lu", p->data_ptr_->size());

    return p;
  }

  void OnRead(T* p) {}

  void OnWrite(T* p) {}

  void OnFlush(shared_ptr<T> p) {

    const std::string hostname("codered");

    Client::Instance().deposit_chunk(hostname, p);

    std::string key(boost::lexical_cast<std::string>(p->cuid_) + p->uid_);

    shared_ptr<C> chunk_info = cache_[key];
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
