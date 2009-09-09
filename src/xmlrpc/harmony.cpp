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
      generate_op(HARMONY_WRITE, key, value, "0.0.0.0:0-0-5")
      );

  xmlrpc_c::paramList param_list;

  // Operand.
  param_list.add(op_param);

  // Timeout.
  param_list.add(xmlrpc_c::value_nil());

  const std::string url("http://barb.cs.washington.edu:36459");
  xmlrpc_c::clientSimple  client;
  xmlrpc_c::value         result;

  client.call(url, HARMONY_APPLY, param_list, &result);

  xmlrpc_c::value_struct op_struct = xmlrpc_c::value_array(result).vectorValueValue()[1];
  std::map<std::string, xmlrpc_c::value> op_map(op_struct);

  xmlrpc_c::value_int e = op_map["err"];

  if (e) {
    std::cout << "Errno: " << e << std::endl;
    throw key_missing_error();
  }

}




std::string harmony::read(const std::string& key) {

  const xmlrpc_c::value_struct op_param(
      generate_op(HARMONY_READ, key, "0.0.0.0:0-0-6")
      );

  xmlrpc_c::paramList param_list;

  // Operand.
  param_list.add(op_param);

  // Timeout.
  param_list.add(xmlrpc_c::value_nil());

  const std::string url("http://barb.cs.washington.edu:36459");
  xmlrpc_c::clientSimple  client;
  xmlrpc_c::value         result;

  client.call(url, HARMONY_APPLY, param_list, &result);

  xmlrpc_c::value_struct op_struct = xmlrpc_c::value_array(result).vectorValueValue()[1];
  std::map<std::string, xmlrpc_c::value> op_map(op_struct);

  xmlrpc_c::value_int e = op_map["err"];

  if (e) {
    std::cout << "Errno: " << e << std::endl;
    throw key_missing_error();
  }

  xmlrpc_c::value_struct value_struct = op_map["value"];

  std::map<std::string, xmlrpc_c::value> value_map(value_struct);
  xmlrpc_c::value_string value = value_map["value"];

  return value.crlfValue();
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
    HarmonyOperation Op,
    const std::string& key,
    const std::string& id) {

  const std::string XML_OP_TAG("op");
  const std::string XML_KEY_TAG("key");
  const std::string XML_ID_TAG("id");

  boost::hash<std::string> string_hash;
  const size_t hash = string_hash(key);

  std::map<std::string, xmlrpc_c::value> op_param_map;
  op_param_map[XML_OP_TAG] = xmlrpc_c::value_int(Op);
  op_param_map[XML_KEY_TAG] = xmlrpc_c::value_int(hash);
  op_param_map[XML_ID_TAG] = xmlrpc_c::value_string(id);

  return op_param_map;
}




std::map<std::string, xmlrpc_c::value> harmony::generate_op(
    HarmonyOperation Op,
    const std::string& key,
    const std::string& value,
    const std::string& id) {

  const std::string XML_VALUE_TAG("value");

  std::map<std::string, xmlrpc_c::value>
  op_param_map(generate_op(Op, key, id));

  std::map<std::string, xmlrpc_c::value> value_param_map;
  value_param_map[XML_VALUE_TAG] = xmlrpc_c::value_string(value);
  const xmlrpc_c::value_struct value_param(value_param_map);

  op_param_map[XML_VALUE_TAG] = value_param;

  return op_param_map;
}




} } // Namespace
