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

#include "bridge.hpp"

#include "../debug.hpp"
#include "../storage/chunk_data.hpp"

#include <boost/smart_ptr.hpp>

namespace colony {

class lookup_e : public boost::exception {};

template<typename T>
class basic_cache {

public:

  typedef typename T::key_type     key_type;
  typedef T                        element_type;
  typedef typename T::value_type   value_type;

  typedef std::map<key_type, boost::shared_ptr<element_type> > whole_type;
  typedef std::map<key_type, boost::weak_ptr<element_type> >   dirty_type;


  basic_cache() {}
  virtual ~basic_cache() {}

  const boost::shared_ptr<element_type>
  read(const key_type key) {

    boost::shared_ptr<element_type> value;

    typename whole_type::iterator it = whole_.find(key);

    if (it == whole_.end()) {

      throw colony::lookup_e();

    } else {
      value = it->second;
    }

    return value;
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


protected:

  whole_type whole_;
  dirty_type dirty_;

};

} // Namespace

#endif /* BASIC_CACHE_HPP_ */
