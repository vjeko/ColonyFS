/*
 * aggregator.hpp
 *
 *  Created on: May 16, 2009
 *      Author: vjeko
 */

#ifndef AGGREGATOR_HPP_
#define AGGREGATOR_HPP_

#include "../xmlrpc/values.hpp"
#include "../xmlrpc/attribute.hpp"

#include <sys/stat.h>

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/exception.hpp>


namespace colony {

template<typename T> class aggregator {

public:

  typedef typename T::key_type           key_type;
  typedef typename T::value_type         value_type;
  typedef boost::shared_ptr<value_type>  value_type_ptr;

  typedef boost::unordered_map<std::string, std::string>  implementation_type;

  typedef typename implementation_type::iterator        iterator;
  typedef typename implementation_type::const_iterator  const_iterator;

  aggregator() {};

  virtual ~aggregator() {};

  inline value_type_ptr operator[](const key_type& key) {
    typename implementation_type::iterator it = implementation_.find(key);

    if(it == implementation_.end())
      throw std::runtime_error("bzzz");

    T object(key, it->second);
    return value_type_ptr( new value_type( object.get_mapped() ) );
  }

  inline void commit(const key_type& key, const value_type& value) {
    T object(key, value);
    implementation_[key] = object.get_value();
  }

  inline void erase(const key_type& key) {
      implementation_.erase(key);
  }

private:

  implementation_type implementation_;
};

} // Namespace.
#endif /* AGGREGATOR_HPP_ */
