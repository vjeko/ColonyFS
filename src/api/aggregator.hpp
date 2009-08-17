/*
 * aggregator.hpp
 *
 *  Created on: May 16, 2009
 *      Author: vjeko
 */

#ifndef AGGREGATOR_HPP_
#define AGGREGATOR_HPP_

#include "../debug.hpp"
#include "../xmlrpc/values.hpp"
#include "../xmlrpc/attribute.hpp"

#include <sys/stat.h>

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/exception.hpp>


namespace colony {




typedef boost::error_info<struct metadata_error, std::string> key_info_t;
class lookup_e : public boost::exception {};




template<typename T> class aggregator {

public:

  typedef typename T::key_type           key_type;
  typedef typename T::value_type         value_type;
  typedef typename T::mapped_type        mapped_type;
  typedef boost::shared_ptr<value_type>  mapped_type_ptr;

  typedef boost::unordered_map<std::string, mapped_type>  cache_type;
  typedef boost::unordered_map<std::string, std::string>  implementation_type;

  typedef typename implementation_type::iterator          iterator;
  typedef typename implementation_type::const_iterator    const_iterator;

  aggregator() {};

  virtual ~aggregator() {};

  inline mapped_type& operator[](const key_type& key) {
    typename implementation_type::iterator it = implementation_.find(key);

    if(it == implementation_.end())
      throw lookup_e() << key_info_t(key);

    T object(key);
    object.set_value(it->second);
    cache_[key] = object.get_mapped();

    return cache_[key];
  }

  inline void commit(const key_type& key, const value_type& value) {
    T object(key, value);
    implementation_[key] = object.get_value();
  }

  inline void erase(const key_type& key) {
      implementation_.erase(key);
      cache_.erase(key);
  }

private:

  cache_type          cache_;
  implementation_type implementation_;
};

} // Namespace.
#endif /* AGGREGATOR_HPP_ */
