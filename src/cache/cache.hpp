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

#include <boost/shared_ptr.hpp>
#include <boost/asio/io_service.hpp>




namespace colony {


template <typename T, typename D >
class cache : private basic_cache<T> {

public:

  typedef typename T::key_type  key_type;
  typedef T                     value_type;

  typedef std::map<key_type, boost::shared_ptr<value_type> > whole_type;
  typedef std::map<key_type, boost::weak_ptr<value_type> >   dirty_type;

  cache() {}

  const boost::shared_ptr<value_type>
  operator()(const key_type key) {

    boost::shared_ptr<T> value(this->read(key));
    boost::shared_ptr<T>
    dummy(
        value.get(),
        boost::bind(&D::OnRead, &deleter_, _1)
    );

    return dummy;

  }

  const boost::shared_ptr<value_type>
  operator[](const key_type key) {

    boost::shared_ptr<T> value(this->mutate(key));
    boost::shared_ptr<T>
    dummy(
        value.get(),
        boost::bind(&D::OnWrite, &deleter_, _1)
    );

    return value;

  }

  void flush() {

    // FIXME: Concurrency issues!

    typename dirty_type::const_iterator it;
    for(it = this->dirty_.begin(); it != this->dirty_.end(); ++it) {
      shared_ptr<T> value = it->second.lock();
      deleter_.OnFlush(value);
    }

    this->dirty_.clear();
    deleter_.OnDone();

  }

  D deleter_;

};


} // Namespace


#endif /* CACHE_HPP_ */
