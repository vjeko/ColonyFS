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

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

#include "../../xmlrpc/harmony.hpp"

/**
 * This test case tests Harmony put and get storage interface
 * consistency.
 */
BOOST_AUTO_TEST_CASE( harmony_xmlrpc_storage ) {

  const int iterations = 100;
  BOOST_TEST_MESSAGE( "\tTesting Harmony put/get interface" );

  try {
    std::string const url("http://tricycle.cs.washington.edu:1212/RPC2");
    colony::xmlrpc::harmony harmony(url);

    std::ostringstream stream;

    /*
     * Math rand() function is being used to generate a
     * random string.
     */
    for(int i = 0; i < iterations; i++) {
      stream << "__________";
      stream << rand();
      stream << rand();
      stream << "___";

      const std::string key = stream.str();
      stream.str("");

      stream << "----------";
      stream << rand();
      stream << rand();
      stream << "---";

      const std::string value = stream.str();
      stream.str("");

      harmony.put(key, value);
      const std::string return_value = harmony.get(key);

      /*
       * Check whether stored and retrieved values are the same.
       */
      BOOST_CHECK(!value.compare(return_value));
    }

  } catch(colony::xmlrpc::key_missing_error& e) {
    BOOST_FAIL( "Key not found." );
  } catch (girerr::error& e) {
    BOOST_FAIL( "Harmony error: " << std::string(e.what()) );
  }

}
