/*
 * harmony.cpp
 *
 *  Created on: Jan 31, 2009
 *      Author: vjeko
 */

#include "harmony.hpp"
#include "../debug.hpp"

#include <iostream>
#include <vector>
#include <map>

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>

#include <boost/functional/hash.hpp>




namespace colony { namespace xmlrpc {

harmony::harmony(std::string url) :
    url_(url) {
}

harmony::~harmony() {}



bool harmony::put(const std::string& key, const std::string& value) {

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
  xmlrpc_c::value         result;

  RetryPut:

  try {
    client_.call(url, METHOD_CALL, param_list, &result);
  } catch (girerr::error& e) {
    sleep(1);

    goto RetryPut;
  }

  xmlrpc_c::value_array response(result);

  xmlrpc_c::value_int dht_errno = response.vectorValueValue()[0];
  xmlrpc_c::value_struct request = response.vectorValueValue()[1];
  xmlrpc_c::value_struct op_struct = response.vectorValueValue()[2];

  std::map<std::string, xmlrpc_c::value> request_map(op_struct);
  std::map<std::string, xmlrpc_c::value> op_struct_map(op_struct);

  if (dht_errno) {
    throw dht_error();
  }

  xmlrpc_c::value_int key_errno = op_struct_map[XML_ERROR_TAG];

  if (key_errno) {
    throw key_missing_error();
  }

  return true;
}




std::vector<unsigned char> harmony::getv(const std::string key) {

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

  /*
   * The reason why we recreate client in every step,
   * is to avoid race conditions. XMRPC documentation
   * does not specify whether clientSimple can be used as
   * a shared object.
   */
  const std::string url("http://barb.cs.washington.edu:36459");
  xmlrpc_c::value         result;

  RetryPut:

  try {
    client_.call(url, METHOD_CALL, param_list, &result);
  } catch (girerr::error& e) {
    sleep(0.2);

    goto RetryPut;
  }


  xmlrpc_c::value_array response(result);


  xmlrpc_c::value_int dht_errno = response.vectorValueValue()[0];
  xmlrpc_c::value_struct request = response.vectorValueValue()[1];
  xmlrpc_c::value_struct op_struct = response.vectorValueValue()[2];

  std::map<std::string, xmlrpc_c::value> request_map(op_struct);
  std::map<std::string, xmlrpc_c::value> op_struct_map(op_struct);

  if (dht_errno) {
    throw dht_error();
  }

  xmlrpc_c::value_int key_errno = op_struct_map[XML_ERROR_TAG];

  if (key_errno) {
    throw key_missing_error();
  }

  xmlrpc_c::value_struct value_struct = op_struct_map[XML_VALUE_TAG];
  std::map<std::string, xmlrpc_c::value> value_map(value_struct);

  xmlrpc_c::value_bytestring raw_value = value_map[XML_VALUE_TAG];
  std::vector<unsigned char> raw_value_vector = raw_value.vectorUcharValue();

  return raw_value_vector;
}



std::string harmony::get(const std::string& key) {

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

  /*
   * The reason why we recreate client in every step,
   * is to avoid race conditions. XMRPC documentation
   * does not specify whether clientSimple can be used as
   * a shared object.
   */
  const std::string url("http://barb.cs.washington.edu:36459");
  xmlrpc_c::value         result;

  RetryPut:

  try {
    client_.call(url, METHOD_CALL, param_list, &result);
  } catch (girerr::error& e) {
    sleep(0.2);

    goto RetryPut;
  }


  xmlrpc_c::value_array response(result);


  xmlrpc_c::value_int dht_errno = response.vectorValueValue()[0];
  xmlrpc_c::value_struct request = response.vectorValueValue()[1];
  xmlrpc_c::value_struct op_struct = response.vectorValueValue()[2];

  std::map<std::string, xmlrpc_c::value> request_map(op_struct);
  std::map<std::string, xmlrpc_c::value> op_struct_map(op_struct);

  if (dht_errno) {
    throw dht_error();
  }

  xmlrpc_c::value_int key_errno = op_struct_map[XML_ERROR_TAG];

  if (key_errno) {
    throw key_missing_error();
  }

  xmlrpc_c::value_struct value_struct = op_struct_map[XML_VALUE_TAG];
  std::map<std::string, xmlrpc_c::value> value_map(value_struct);

  xmlrpc_c::value_bytestring raw_value = value_map[XML_VALUE_TAG];
  std::vector<unsigned char> raw_value_vector = raw_value.vectorUcharValue();

  std::string value(raw_value_vector.begin(), raw_value_vector.end());
  return value;
}




std::map<std::string, xmlrpc_c::value> harmony::generate_op(
    const std::string& key) {

  boost::hash<std::string> string_hash;
  const int hash = (unsigned short) string_hash(key);

  rInfo("Hash: (%d)", hash);

  std::map<std::string, xmlrpc_c::value> op_param_map;
  op_param_map[XML_KEY_TAG] = xmlrpc_c::value_int(hash);

  return op_param_map;
}




std::map<std::string, xmlrpc_c::value> harmony::generate_op(
    const std::string& key,
    const std::vector<unsigned char>& value) {

  std::map<std::string, xmlrpc_c::value>
  op_param_map(generate_op(key));

  std::map<std::string, xmlrpc_c::value> value_param_map;
  value_param_map[XML_VALUE_TAG] = xmlrpc_c::value_bytestring(value);
  const xmlrpc_c::value_struct value_param(value_param_map);

  op_param_map[XML_VALUE_TAG] = value_param;

  return op_param_map;
}




std::map<std::string, xmlrpc_c::value> harmony::generate_op(
    const std::string& key,
    const std::string& value) {

  std::map<std::string, xmlrpc_c::value>
  op_param_map(generate_op(key));

  std::map<std::string, xmlrpc_c::value> value_param_map;
  std::vector<unsigned char> raw_value(value.begin(), value.end());
  value_param_map[XML_VALUE_TAG] = xmlrpc_c::value_bytestring(raw_value);
  const xmlrpc_c::value_struct value_param(value_param_map);

  op_param_map[XML_VALUE_TAG] = value_param;

  return op_param_map;
}




void harmony::validate() {
}




} } // Namespace
