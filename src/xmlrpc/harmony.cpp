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




const std::string XML_OP_TAG("op");
const std::string XML_VALUE_TAG("value");
const std::string XML_KEY_TAG("key");
const std::string XML_ID_TAG("id");

const std::string HARMONY_APPLY("op_apply");

const int HARMONY_READ = 0;
const int HARMONY_WRITE = 1;




namespace colony { namespace xmlrpc {

harmony::harmony(std::string url) :
    url_(url) {
}

harmony::~harmony() {}


void harmony::write(const std::string& key, const std::string& value) {
   using std::string;
   using std::map;
   using std::pair;
   using xmlrpc_c::value_int;
   using xmlrpc_c::value_string;
   using xmlrpc_c::value_struct;
   using xmlrpc_c::paramList;


   map<string, xmlrpc_c::value> value_param_map;
   value_param_map[XML_VALUE_TAG] = xmlrpc_c::value_string(value);

   const value_struct value_param(value_param_map);


   boost::hash<std::string> string_hash;
   const size_t hash = string_hash(key);

   map<string, xmlrpc_c::value> op_param_map;
   op_param_map[XML_OP_TAG] = xmlrpc_c::value_int(HARMONY_WRITE);
   op_param_map[XML_KEY_TAG] = xmlrpc_c::value_int(hash);
   op_param_map[XML_ID_TAG] = xmlrpc_c::value_string("0.0.0.0:0-0-3");
   op_param_map[XML_VALUE_TAG] = value_param;

   const xmlrpc_c::value_struct op_param(op_param_map);


   paramList param_list;
   param_list.add(op_param);
   param_list.add(xmlrpc_c::value_nil());


   const std::string url("http://barb.cs.washington.edu:36459");
   xmlrpc_c::clientSimple  client;
   xmlrpc_c::value         result;

   client.call(url, HARMONY_APPLY, param_list, &result);
 }




 void harmony::read(const std::string& key) {
   using std::string;
   using std::map;
   using std::pair;
   using xmlrpc_c::value_int;
   using xmlrpc_c::value_string;
   using xmlrpc_c::value_struct;
   using xmlrpc_c::paramList;

   boost::hash<std::string> string_hash;
   const size_t hash = string_hash(key);

   map<string, xmlrpc_c::value> op_param_map;
   op_param_map[XML_OP_TAG] = xmlrpc_c::value_int(HARMONY_READ);
   op_param_map[XML_KEY_TAG] = xmlrpc_c::value_int(hash);
   op_param_map[XML_ID_TAG] = xmlrpc_c::value_string("0.0.0.0:0-0-4");

   const xmlrpc_c::value_struct op_param(op_param_map);


   paramList param_list;
   param_list.add(op_param);
   param_list.add(xmlrpc_c::value_nil());


   const std::string url("http://barb.cs.washington.edu:36459");
   xmlrpc_c::clientSimple  client;
   xmlrpc_c::value         result;

   client.call(url, HARMONY_APPLY, param_list, &result);
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

} } // Namespace
