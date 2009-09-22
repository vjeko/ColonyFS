/*
 * ValueFactory.hpp
 *
 *  Created on: Sep 21, 2009
 *      Author: vjeko
 */

#ifndef VALUEFACTORY_HPP_
#define VALUEFACTORY_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/assert.hpp>

#include "../storage/chunk_data.hpp"
#include "../xmlrpc/values.hpp"




template<typename T>
struct ValueFactory {

  typedef T element_type;




  static element_type Reconstruct(std::string key, std::string value) {

    element_type result;
    result.set_key(key);
    result.set_value(value);

    return result;
  }




  static boost::shared_ptr<element_type> NewPointer(std::string key) {

    boost::shared_ptr<element_type> result = boost::make_shared<T>();
    result->set_instruction(key);

    return result;
  }

};




template<>
struct ValueFactory<colony::storage::chunk_data> {

  typedef colony::storage::chunk_data element_type;




  static boost::shared_ptr<element_type> NewPointer(element_type::key_type key) {

    return boost::make_shared<element_type>(key);
  }

};


#endif /* VALUEFACTORY_HPP_ */
