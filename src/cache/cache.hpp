/*
 * cache.hpp
 *
 *  Created on: Sep 14, 2009
 *      Author: vjeko
 */

#ifndef CACHE_HPP_
#define CACHE_HPP_

#include "basic_cache.hpp"

#include "../xmlrpc/harmony.hpp"

#include "../intercom/user_harmony.hpp"
#include "../accessor.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/bind/protect.hpp>
#include <boost/asio/io_service.hpp>




namespace colony {


template <typename T, typename Policy >
class cache {

public:

  typedef typename T::key_type  key_type;
  typedef T                     value_type;

  typedef typename basic_cache<T>::whole_type   whole_type;
  typedef typename basic_cache<T>::dirty_type   dirty_type;




  cache() {}




  const boost::shared_ptr<value_type>
  operator()(const key_type key) {

    boost::shared_ptr<T> value;

    try {
      rDebug("Reading data from the remote server.");
      value = policy_.PreRead(key);
    } catch(colony::lookup_e& e) {
      rDebug("Reading data from the cache.");
      value = cache_impl_.read(key);

    }

    return value;
  }




  const boost::shared_ptr<value_type>
  operator[](const key_type key) {

    boost::shared_ptr<T> value = cache_impl_.mutate(key);
    boost::shared_ptr<T>
    dummy(
        value.get(),
        boost::bind(&Policy::OnWrite, &policy_, _1)
    );

    return value;

  }




  void flush() {

    // FIXME: Concurrency issues!

    typename dirty_type::const_iterator it;
    for(it = cache_impl_.dirty_.begin(); it != cache_impl_.dirty_.end(); ++it) {
      shared_ptr<T> value = it->second.lock();
      policy_.OnFlush(value);
    }

    cache_impl_.dirty_.clear();
    policy_.OnDone();

  }




  Policy                       policy_;
  colony::basic_cache<T>       cache_impl_;

};


} // Namespace


#endif /* CACHE_HPP_ */
