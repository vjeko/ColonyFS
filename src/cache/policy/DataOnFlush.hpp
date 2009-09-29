/*
 * DataOnFlush.hpp
 *
 *  Created on: Sep 19, 2009
 *      Author: vjeko
 */

#ifndef DATAONFLUSH_HPP_
#define DATAONFLUSH_HPP_

#include <boost/noncopyable.hpp>

#include <boost/weak_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <set>

#include "MetadataOnFlush.hpp"

#include "../cache.hpp"
#include "../../accessor.hpp"
#include "../../algo/CSScheduler.hpp"




template <typename T>
struct DataOnFlush : boost::noncopyable {

  typedef colony::xmlrpc::chunk_value C;

  typedef std::set< boost::weak_ptr<T> > set_type;

  DataOnFlush() {

	  Client::Thread();

  }




  void PreRead(shared_ptr<T> p) {

    const std::string hostname = colony::CSScheduler::GetCS();

    Client::Instance().retrieve_chunk(hostname, p);

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
  }



  boost::unordered_map<
	  std::string,
	  set_type
  > track_;

  colony::cache<C, MetadataOnFlush<C> > cache_;

};

#endif /* DATAONFLUSH_HPP_ */
