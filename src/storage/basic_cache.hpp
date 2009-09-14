/*
 * cache.hpp
 *
 *  Created on: Sep 13, 2009
 *      Author: vjeko
 */

#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <utility>

#include <boost/unordered_map.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "bridge.hpp"

#include "../debug.hpp"
#include "../storage/chunk_data.hpp"

template<typename T>
class basic_cache {
public:

  typedef std::string      key_type;
  typedef T                value_type;

  typedef boost::unordered_map<key_type, boost::shared_ptr<value_type> > whole_type;
  typedef boost::unordered_map<key_type, boost::weak_ptr<value_type> >   dirty_type;


  basic_cache() {}
  virtual ~basic_cache() {}

  const boost::shared_ptr<value_type>
  read(
      const typename value_type::uid_type filename,
      const typename value_type::cuid_type count) {

    const std::string key = filename + boost::lexical_cast<std::string>(count);
    boost::shared_ptr<value_type> value;

    typename whole_type::iterator it = whole_.find(key);

    if (it == whole_.end()) {
      value = boost::shared_ptr<value_type>(new value_type(filename, count));
      whole_[key] = value;
    } else {
      value = it->second;
    }

    return value;
  }




  const boost::shared_ptr<value_type>
  mutate(
      const typename value_type::uid_type filename,
      const typename value_type::cuid_type count) {

    const std::string key = filename + boost::lexical_cast<std::string>(count);
    boost::shared_ptr<value_type> value;

    typename whole_type::iterator it = whole_.find(key);

    if (it == whole_.end()) {
      value = boost::shared_ptr<value_type>(new value_type(filename, count));
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

#endif /* CACHE_HPP_ */
