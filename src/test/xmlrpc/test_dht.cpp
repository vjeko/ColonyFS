/*
 * sample.cpp
 *
 *  Created on: Jan 25, 2009
 *      Author: vjeko
 */
#define BOOST_TEST_MODULE harmony_xmlrpc
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <sstream>

#include "../../xmlrpc/harmony.hpp"

/**
 * Test Harmony's put and get storage interface consistency.
 */
BOOST_AUTO_TEST_CASE( harmony_xmlrpc_storage ) {

  BOOST_TEST_MESSAGE( "Testing Harmony put/get interface." );

  std::string url("http://barb.cs.washington.edu:36459");
  const int iteration_limit = 20;

  colony::xmlrpc::harmony dht(url);

  std::ostringstream oss_key;
  std::ostringstream oss_value;

  oss_value << "VALUE_";

  for(int i = 0; i < iteration_limit; i++) {

    oss_key << i;
    oss_value << i;

    std::string key = oss_key.str();
    std::string value = oss_value.str();

    dht.put(key, value);
    std::string result = dht.get(key);

    if (result.compare(value))
      BOOST_FAIL("Retrieved and put value are not the same.");

  }

  try {

  } catch(colony::xmlrpc::key_missing_error& e) {
    BOOST_FAIL( "Key not found.");
  } catch(colony::xmlrpc::dht_error& e) {
    BOOST_FAIL( "DHT error.");
  } catch (girerr::error& e) {
    BOOST_FAIL( "XML-RPC error: " << std::string(e.what()) );
  }

}
