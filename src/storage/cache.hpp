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


namespace colony {


template <typename T>
struct Deleter {

  struct OnRead {
    void operator()(T* p) {

      std::cout << "\tOnRead Deleter Called!" << std::endl;

    }
  };

  struct OnWrite {
    void operator()(T* p) {

      std::cout << "\tOnWrite Deleter Called!" << std::endl;

    }
  };

  struct OnFlush {
    void operator()(T* p) {

      std::cout << "\tOnFlush Deleter Called!" << std::endl;

    }
  };

};


template <typename T>
class cache : private basic_cache<T> {

public:

  typedef typename T::key_type  key_type;
  typedef T                     value_type;

  cache() : dht_("harmony_test") {}

  const boost::shared_ptr<value_type>
  operator()(const key_type key) {

    boost::shared_ptr<T> value(this->read(key));
    boost::shared_ptr<T> dummy(value.get(), typename Deleter<T>::OnRead());

    return dummy;

  }

  const boost::shared_ptr<value_type>
  operator[](const key_type key) {

    boost::shared_ptr<T> value(this->mutate(key));
    boost::shared_ptr<T> dummy(value.get(), typename Deleter<T>::OnWrite());

    return dummy;

  }


  colony::xmlrpc::harmony  dht_;

};


} // Namespace


#endif /* CACHE_HPP_ */
