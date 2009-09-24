/*
 * cache.hpp
 *
 *  Created on: Sep 13, 2009
 *      Author: vjeko
 */

#ifndef BASIC_CACHE_HPP_
#define BASIC_CACHE_HPP_

#include <utility>

#include <boost/unordered_map.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple_io.hpp>

#include "../debug.hpp"
#include "../exceptions.hpp"
#include "../storage/chunk_data.hpp"

#include <boost/smart_ptr.hpp>

namespace colony {

template<typename T>
class basic_cache {

public:

  typedef T                        element_type;
  typedef typename T::key_type     key_type;
  typedef typename T::value_type   value_type;

  typedef std::map<key_type, boost::shared_ptr<element_type> > whole_type;
  typedef std::map<key_type, boost::weak_ptr<element_type> >   dirty_type;


  basic_cache() {}
  virtual ~basic_cache() {}


  void read(shared_ptr<T> value) {

    typename element_type::key_type key = value->get_key();
    typename whole_type::iterator it = whole_.find(key);

    if (it == whole_.end()) {

      rWarning("Value not in the cache!");
      throw colony::cache_miss_e();

    } else {

      (*value) = (*whole_[key]);

    }

  }



  void mutate(shared_ptr<T> value) {

    typename T::key_type key = value->get_key();
    typename whole_type::iterator it = whole_.find(key);


    if (it == whole_.end()) {

      whole_[key] = boost::shared_ptr<T>(value);

    } else {

      value = boost::shared_ptr<T>(it->second);

    }

    dirty_[key] = boost::shared_ptr<T>(value);

  }




  const boost::shared_ptr<element_type>
  mutate(const key_type key) {

    boost::shared_ptr<element_type> value;

    typename whole_type::iterator it = whole_.find(key);

    if (it == whole_.end()) {

      value = boost::shared_ptr<element_type>(new element_type(key));
      whole_[key] = value;

    } else {
      value = it->second;
    }

    dirty_[key] = value;
    return value;
  }




  const boost::shared_ptr<element_type>
  read(const key_type key) {

    boost::shared_ptr<element_type> value;

    typename whole_type::iterator it = whole_.find(key);

    if (it == whole_.end()) {

      rWarning("Value not in the cache!");
      throw colony::cache_miss_e();

    } else {
      value = it->second;
    }

    return value;
  }




  whole_type whole_;
  dirty_type dirty_;

};

} // Namespace

#endif /* BASIC_CACHE_HPP_ */
