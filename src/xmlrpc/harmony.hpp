/*
 * harmony.hpp
 *
 *  Created on: Jan 31, 2009
 *      Author: vjeko
 */

#ifndef HARMONY_HPP_
#define HARMONY_HPP_

#include "values.hpp"

#include <boost/exception.hpp>

#include <xmlrpc-c/base.hpp>

#include <string>
#include <map>
#include <iostream>
#include <sstream>




enum HarmonyOperation {
  HARMONY_READ,
  HARMONY_WRITE
};





namespace colony { namespace xmlrpc {

typedef boost::error_info<struct tag_key, std::string> key_info;
typedef boost::error_info<struct tag_key, std::string> dht_info;

class key_missing_error : public boost::exception {};
class dht_error : public boost::exception {};





class harmony {
protected:

  typedef std::string key_t;
  typedef std::string value_t;

public:
  harmony(std::string url);
  virtual ~harmony();

  bool put(const std::string&, const std::string&);
  std::string get(const std::string&);

  /*
   * Get value.
   *
   * Do not deal with exceptions at this level. Let caller take an
   * appropriate action.
   */
  template <typename T>
  T get_value (xmlrpc::instruction instruction) {

    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);

    oa << instruction;

    key_t    key    = ss.str();
    value_t  value  = get(key);

    return T(key, value);
  }

  template <typename T>
  T get_value (std::string key) {

    value_t  value  = get(key);

    return T(key, value);
  }

  // Set key value pair.
  template <typename T>
  void set_pair (T& pair) {
    put( pair.get_key(), pair.get_value() );
  }

private:


  std::map<std::string, xmlrpc_c::value> generate_op(
      const std::string& key);

  std::map<std::string, xmlrpc_c::value> generate_op(
      const std::string& key,
      const std::string& value);

  void validate(xmlrpc_c::value_array result);


  static const int  port_ = 36459;
  std::string       url_;
};

} } // Namespace

#endif /* HARMONY_HPP_ */
