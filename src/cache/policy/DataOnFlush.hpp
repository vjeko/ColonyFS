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

    std::string key(boost::lexical_cast<std::string>(p->cuid_) + p->uid_);

    shared_ptr<C> cs_info(new C(key));
    DHT::Instance().get_pair(cs_info);

    const std::string hostname = cs_info->get_mapped();

    Client::Instance().retrieve_chunk(hostname, p);


  }

  void OnRead(T* p) {}




  void OnWrite(T* p) {}





  void OnErase(shared_ptr<T> p) {}




  void OnFlush(shared_ptr<T> p) {

    const std::string hostname = colony::CSScheduler::GetCS();

    Client::Instance().deposit_chunk(hostname, p);

    std::string key(boost::lexical_cast<std::string>(p->cuid_) + p->uid_);

    shared_ptr<C> chunk_info = ValueFactory<C>::NewPointer(key);
    chunk_info->set_mapped(hostname);
    DHT::Instance().set_pair(chunk_info);
  }

  void OnDone() {
    cache_.invalidate();
  }



  boost::unordered_map<
	  std::string,
	  set_type
  > track_;

  colony::cache<C, MetadataOnFlush<C> > cache_;

};

#endif /* DATAONFLUSH_HPP_ */
