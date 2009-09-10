/*
 * harmony.cpp
 *
 *  Created on: Jan 31, 2009
 *      Author: vjeko
 */

#include "harmony.hpp"

#include <iostream>
#include <vector>
#include <map>

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

#include <boost/functional/hash.hpp>




const std::string HARMONY_APPLY("op_apply");




namespace colony { namespace xmlrpc {

harmony::harmony(std::string url) :
    url_(url) {
}

harmony::~harmony() {}



void harmony::write(const std::string& key, const std::string& value) {

  const xmlrpc_c::value_struct op_param(
      generate_op(key, value)
      );

  xmlrpc_c::paramList param_list;

  // Operation Type.
  param_list.add(xmlrpc_c::value_int(HARMONY_WRITE));

  // Operand.
  param_list.add(op_param);

  // Timeout.
  param_list.add(xmlrpc_c::value_nil());

  // Commit.
  param_list.add(xmlrpc_c::value_nil());

  const std::string url("http://barb.cs.washington.edu:36459");
  xmlrpc_c::clientSimple  client;
  xmlrpc_c::value         result;

  client.call(url, HARMONY_APPLY, param_list, &result);
/*
  xmlrpc_c::value_array response(result);

  xmlrpc_c::value_int dht_error = response.vectorValueValue()[0];
  xmlrpc_c::value_struct op_struct = response.vectorValueValue()[1];

  std::map<std::string, xmlrpc_c::value> op_map(op_struct);
  xmlrpc_c::value_int key_error = op_map[XML_ERROR_TAG];
*/

}




std::string harmony::read(const std::string& key) {

  const xmlrpc_c::value_struct op_param(
      generate_op(key)
      );

  xmlrpc_c::paramList param_list;

  // Operation Type.
  param_list.add(xmlrpc_c::value_int(HARMONY_READ));

  // Operand.
  param_list.add(op_param);

  // Timeout.
  param_list.add(xmlrpc_c::value_nil());

  // Commit.
  param_list.add(xmlrpc_c::value_nil());

  const std::string url("http://barb.cs.washington.edu:36459");
  xmlrpc_c::clientSimple  client;
  xmlrpc_c::value         result;

  client.call(url, HARMONY_APPLY, param_list, &result);

  xmlrpc_c::value_struct op_struct = xmlrpc_c::value_array(result).vectorValueValue()[1];
  std::map<std::string, xmlrpc_c::value> op_map(op_struct);
/*
  xmlrpc_c::value_int e = op_map["err"];

  if (e) {
    std::cout << "Errno: " << e << std::endl;
    return "";
  }

  xmlrpc_c::value_struct value_struct = op_map["value"];

  std::map<std::string, xmlrpc_c::value> value_map(value_struct);
  xmlrpc_c::value_string value = value_map["value"];

  return value.crlfValue();
*/
}




bool harmony::put(const std::string& key, const std::string& value) {

  /*
   * The reason why we recreate client in every step,
   * is to avoid race conditions. XMRPC documentation
   * does not specify whether clientSimple can be used as
   * a shared object.
   */
  xmlrpc_c::clientSimple  client;
  xmlrpc_c::value         result;
  std::string             method("write");

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
  std::string             method("read");

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




std::map<std::string, xmlrpc_c::value> harmony::generate_op(
    const std::string& key) {

  const std::string XML_KEY_TAG("key");

  boost::hash<std::string> string_hash;
  const size_t hash = string_hash(key);

  std::map<std::string, xmlrpc_c::value> op_param_map;
  op_param_map[XML_KEY_TAG] = xmlrpc_c::value_int(hash);

  return op_param_map;
}




std::map<std::string, xmlrpc_c::value> harmony::generate_op(
    const std::string& key,
    const std::string& value) {

  const std::string XML_VALUE_TAG("value");

  std::map<std::string, xmlrpc_c::value>
  op_param_map(generate_op(key));

  std::map<std::string, xmlrpc_c::value> value_param_map;
  value_param_map[XML_VALUE_TAG] = xmlrpc_c::value_string(value);
  const xmlrpc_c::value_struct value_param(value_param_map);

  op_param_map[XML_VALUE_TAG] = value_param;

  return op_param_map;
}




void harmony::validate(xmlrpc_c::value_array result) {

}




} } // Namespace
