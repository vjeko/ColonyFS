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



  void insert(shared_ptr<T>& value) {

    const key_type key = value->get_key();

    /*
     * When using a default constructor (such as with STL containers),
     * the use count is zero. This means we must not use reset, but
     * instead we have to resort to a copy constructor, which will set the
     * use count to one and validate the pointer.
     */
    whole_[key] = shared_ptr<T>(value.get());

  }




  /* Label that entry as dirty. */
  void set_dirty(shared_ptr<T>& value) {

    const key_type key = value->get_key();

    dirty_[key] = whole_[key];

  }




  void read(shared_ptr<T>& value) {

    const key_type key = value->get_key();

    typename whole_type::iterator it = whole_.find(key);

    if (it == whole_.end()) {

      rWarning("Value not in the cache!");
      throw colony::cache_miss_e();

    } else {

      /*
       * Reset the value while retaining the use count and the deleter.
       * However, make sure to validate the pointer.
       *
       * Also, we have to make sure not to interfere with the underlying
       * cache implementation. Thus, only <reset(shared_ptr<T>, T*)> can
       * be used.
       */
      value.reset(value, it->second.get());

    }

    /* Let's make sure that everything is in order. */
    BOOST_ASSERT(whole_[key].use_count() != 0);
    BOOST_ASSERT(value.use_count() != 0);

  }


  void erase(shared_ptr<T>& value) {
    whole_.erase(value->get_key());
    dirty_.erase(value->get_key());
  }


  void clear(shared_ptr<T>& value) {
	  dirty_.erase(value->get_key());
  }


  void clear() {
    whole_.clear();
    dirty_.clear();
  }



  whole_type whole_;
  dirty_type dirty_;

};

} // Namespace

#endif /* BASIC_CACHE_HPP_ */
