/*
 * harmony.cpp
 *
 *  Created on: Jan 31, 2009
 *      Author: vjeko
 */

#include "harmony.hpp"

#include <iostream>
#include <vector>

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

namespace uledfs { namespace xmlrpc {

harmony::harmony(std::string url) :
    url_(url) {
}

harmony::~harmony() {}

bool harmony::put(const std::string& key, const std::string& value) {

  /*
   * The reason why we recreate client in every step,
   * is to avoid race conditions. XMRPC documentation
   * does not specify whether clientSimple can be used as
   * a shared object.
   */
  xmlrpc_c::clientSimple  client;
  xmlrpc_c::value         result;
  std::string             method("put");

  client.call(url_, method, "ss", &result, key.c_str(), value.c_str());

  /*
   * The return is an array.
   * [1] success.
   * [2] output (if any).
   *
   * For puts there is never any output.
   * For gets there is output only on success.
   */
  //xmlrpc_c::value status = xmlrpc_c::value_array(result).vectorValueValue()[0];

  return xmlrpc_c::value_boolean(result);
}


std::string harmony::get(const std::string& key) {

  /*
   * The reason why we recreate client in every step,
   * is to avoid race conditions. XMRPC documentation
   * does not specify whether clientSimple can be used as
   * a shared object.
   */
  xmlrpc_c::clientSimple  client;
  xmlrpc_c::value         result;
  std::string             method("get");

  client.call(url_, method, "s", &result, key.c_str());

  /* What is the return status? */
  xmlrpc_c::value status_xmlrpc = xmlrpc_c::value_array(result).vectorValueValue()[0];

  /*
   * Throw an exception in case we failed.
   * Let the caller decide what to do.
   */
  if (!xmlrpc_c::value_boolean(status_xmlrpc)) {
    throw key_missing_error() << key_info(key);
  }

  /* Otherwise return the value. */
  xmlrpc_c::value value = xmlrpc_c::value_array(result).vectorValueValue()[1];
  return xmlrpc_c::value_string(value);

}

} } // Namespace
