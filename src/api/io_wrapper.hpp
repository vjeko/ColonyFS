/*
 * io_wrapper.hpp
 *
 *  Created on: May 13, 2009
 *      Author: vjeko
 */

#ifndef IO_WRAPPER_HPP_
#define IO_WRAPPER_HPP_

#include "../xmlrpc/values.hpp"
#include "../xmlrpc/attribute.hpp"

#include <sys/stat.h>

#include <map>
#include <boost/unordered_map.hpp>
#include <boost/exception.hpp>

namespace colony {

typedef boost::error_info<struct metadata_error, std::string> key_info_t;
class lookup_e : public boost::exception {};

template<typename Key,typename T>
class io_wrapper {
private:

  typedef std::string _mapped_type;
  typedef boost::unordered_map<Key, T> implementation_type;

  implementation_type implementation_;

public:

  typedef Key key_type;
  typedef T mapped_type;
  typedef std::pair<const Key, T> value_type;
  typedef boost::shared_ptr<T> mapped_type_ptr;

  typedef typename implementation_type::iterator iterator;
  typedef typename implementation_type::const_iterator const_iterator;

  io_wrapper() {};

  virtual ~io_wrapper() {};

  inline mapped_type& operator[](const key_type& key) {
    typename implementation_type::iterator it = implementation_.find(key);

    if(it == implementation_.end())
      throw lookup_e() << key_info_t(key);

    mapped_type& value = it->second;
    return value;
  }

  inline const mapped_type& operator[](const key_type& key) const {
    typename implementation_type::const_iterator it = implementation_.find(key);

    if(it == implementation_.end())
      throw lookup_e() << key_info_t(key);

    const mapped_type& value = it->second;
    return value;
  }

  inline void commit(const key_type& key, const mapped_type& a) {
    implementation_[key] = a;
  }

  inline void erase(const key_type& key) {
      implementation_.erase(key);
  }

};

} // Namespace.

#endif /* IO_WRAPPER_HPP_ */
