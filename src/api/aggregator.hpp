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

#include <iostream>
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

  typedef boost::unordered_map<std::string, std::string>  implementation_type;
  typedef typename implementation_type::iterator          iterator;
  typedef typename implementation_type::const_iterator    const_iterator;




  aggregator() {};




  virtual ~aggregator() {};




  inline shared_ptr<T> operator()(const key_type& key) {
    shared_ptr<T> pair = make_shared<T>(key);
    typename implementation_type::iterator it = implementation_.find(pair->get_key());

    if(it == implementation_.end())
      throw lookup_e() << key_info_t(pair->get_key());

    pair->set_value(it->second);

    return pair;
  }




  inline void commit(shared_ptr<T> pair) {
    num_++;
    std::cout << "+++++++\t" << num_ << std::endl;;
    implementation_[pair->get_key()] = pair->get_value();
  }




  inline void erase(shared_ptr<T> object) {
    implementation_.erase(object->get_key());
  }




  inline void erase(const key_type& key) {
    T object(key);
    implementation_.erase(object.get_key());
  }




private:

  size_t              num_;
  implementation_type implementation_;
};

} // Namespace.
#endif /* AGGREGATOR_HPP_ */
