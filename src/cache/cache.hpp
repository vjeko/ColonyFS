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
#include "../synchronization.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/asio/io_service.hpp>

#include <boost/spirit/include/phoenix.hpp>


namespace colony {


template <typename T, typename Policy >
class cache {

public:

  typedef cache<T, Policy>      this_type;

  typedef typename T::key_type  key_type;
  typedef T                     value_type;

  typedef typename basic_cache<T>::whole_type   whole_type;
  typedef typename basic_cache<T>::dirty_type   dirty_type;

  typedef colony::basic_cache<T>                cache_impl;




  cache() {}




  const boost::shared_ptr<value_type>
  operator()(const key_type key) {

    using namespace boost::phoenix;
    using namespace boost::phoenix::arg_names;

    boost::shared_ptr<T> value =
        ValueFactory<T>::NewPointer(key, bind(&Policy::OnRead, policy_, arg1));

    try {

      Sync::Lock(value->get_key());

      cache_impl_.read(value);

      Sync::Unlock(value->get_key());

    } catch(colony::cache_miss_e& e) {

      // Read and then insert. Not the other way around.
      policy_.PreRead(value);

      Sync::Lock(value->get_key());

      cache_impl_.insert(value);

      Sync::Unlock(value->get_key());

    }

    return boost::shared_ptr<value_type>(value);
  }




  const boost::shared_ptr<value_type>
  operator[](const key_type key) {

    using namespace boost::phoenix;
    using namespace boost::phoenix::arg_names;

    boost::shared_ptr<T> value =
        ValueFactory<T>::NewPointer(key, bind(&Policy::OnWrite, policy_, arg1));

    try {
      cache_impl_.read(value);
    } catch (colony::cache_miss_e& e) {
      cache_impl_.insert(value);
    }

    cache_impl_.set_dirty(value);

    return boost::shared_ptr<value_type>(value);
  }




  void erase(const key_type key) {

    using namespace boost::phoenix;
    using namespace boost::phoenix::arg_names;

    boost::shared_ptr<T> value =
        ValueFactory<T>::NewPointer(key, bind(&Policy::OnRead, policy_, arg1));

   cache_impl_.erase(value);
   policy_.OnErase(value);
  }


  void clear() {
    cache_impl_.clear();
  }


  void flush() {

    // FIXME: Concurrency issues!

    typename dirty_type::const_iterator it;
    for(it = cache_impl_.dirty_.begin(); it != cache_impl_.dirty_.end(); ++it) {

      shared_ptr<T> value = it->second.lock();

      BOOST_ASSERT(value.use_count() > 1);

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
