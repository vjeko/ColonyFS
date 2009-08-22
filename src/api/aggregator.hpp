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

  typedef boost::unordered_map<std::string, mapped_type>  cache_type;
  typedef boost::unordered_map<std::string, std::string>  implementation_type;

  typedef typename implementation_type::iterator          iterator;
  typedef typename implementation_type::const_iterator    const_iterator;




  aggregator() {};




  virtual ~aggregator() {};




  inline mapped_type& operator[](const key_type& key) {
    T object(key);
    typename implementation_type::iterator it = implementation_.find(object.get_key());

    if(it == implementation_.end())
      throw lookup_e() << key_info_t(key);

    object.set_value(it->second);
    cache_[key] = object.get_mapped();

    return cache_[key];
  }




  inline void commit(const key_type& key, const value_type& value) {
    T object(key, value);
    implementation_[object.get_key()] = object.get_value();
  }




  inline shared_ptr<T> operator()(const key_type& key) {
    shared_ptr<T> pair = make_shared<T>(key);
    typename implementation_type::iterator it = implementation_.find(pair->get_key());

    if(it == implementation_.end())
      throw lookup_e() << key_info_t(pair->get_key());

    pair->set_value(it->second);

    return pair;
  }




  inline void commit(shared_ptr<T> pair) {
    implementation_[pair->get_key()] = pair->get_value();
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
