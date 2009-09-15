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
#include "xmlrpc-c/girerr.hpp"

#include <string>
#include <map>
#include <iostream>
#include <sstream>





namespace colony { namespace xmlrpc {

typedef boost::error_info<struct tag_key, std::string> key_info;
typedef boost::error_info<struct tag_key, std::string> dht_info;

class key_missing_error : public boost::exception {};
class dht_error : public boost::exception {};

enum HarmonyOperation {
  HARMONY_READ,
  HARMONY_WRITE
};

enum HarmonyError {
  E_SUCCESS,
  E_FAIL,
  E_NOT_IMPLEMENTED,
  E_API,
  E_FORMAT,
  E_PENDING,
  E_TIMEOUT,
  E_RETRY,
  E_NOT_FOUND,
  E_CONFIG,
  E_BAD_PACKET,
  E_BAD_CONNECT,
  E_NOT_CONNECTED,
  E_NOT_RUNNING,
  E_ADDRINUSE,
  E_SHUTDOWN
};

const std::string XML_KEY_TAG("key");
const std::string XML_VALUE_TAG("value");
const std::string METHOD_CALL("op_create");
const std::string XML_ERROR_TAG("err");


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

    const std::string raw_key = T::get_signature(key);
    value_t value = get(raw_key);

    return T(raw_key, value);
  }

  template <typename T>
  const boost::shared_ptr<T> get_pair(std::string key) {
    const std::string raw_key = T::get_signature(key);
    value_t value = get(raw_key);

    return boost::shared_ptr<T>(new T(raw_key, value));
  }

  // Set key value pair.
  template <typename T>
  void set_pair (T& pair) {
    put( pair.get_key(), pair.get_value() );
  }

  template <typename T>
  void set_pair (boost::shared_ptr<T> pair_ptr) {
    put( pair_ptr->get_key(), pair_ptr->get_value() );
  }

private:


  std::map<std::string, xmlrpc_c::value> generate_op(
      const std::string& key);

  std::map<std::string, xmlrpc_c::value> generate_op(
      const std::string& key,
      const std::string& value);

  void validate();


  static const int  port_ = 36459;
  std::string       url_;
};

} } // Namespace

#endif /* HARMONY_HPP_ */
