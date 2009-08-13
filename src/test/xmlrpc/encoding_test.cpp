/*
 * encoding_test.cpp
 *
 *  Created on: Feb 3, 2009
 *      Author: vjeko
 */

#define BOOST_TEST_MODULE encoding_xmlrpc
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>
#include <boost/foreach.hpp>

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

#include "../../xmlrpc/values.hpp"

/**
 * This test case tests Harmony put and get storage interface
 * consistency.
 */
BOOST_AUTO_TEST_CASE( encoding_xmlrpc ) {

  uledfs::xmlrpc::filename_value file("filename", 20);
  std::cout << file.get_key() << file.get_value() << std::endl;

  uledfs::xmlrpc::chunk_value chunk("filename", 20, "codered.cs.washington.edu");
  std::cout << chunk.get_key() << chunk.get_value() << std::endl;

  std::string swarm("swarm-name");

  uledfs::xmlrpc::chunkserver_value::list_t chunkservers;
  chunkservers.push_back("hostname1");
  chunkservers.push_back("hostname2");
  chunkservers.push_back("hostname3");

  uledfs::xmlrpc::chunkserver_value chunkserver_swarm(swarm, chunkservers);
  std::string key_value(chunkserver_swarm.get_key() + chunkserver_swarm.get_value());

  std::cout << key_value << std::endl;

  chunkserver_swarm = uledfs::xmlrpc::chunkserver_value(chunkserver_swarm.get_key(), chunkserver_swarm.get_value());
  chunkservers      = uledfs::xmlrpc::chunkserver_value::list_t (chunkserver_swarm.get_chunkservers());

  BOOST_FOREACH(std::string& host, chunkservers) {
    std::cout << "Host: " << host << std::endl;
  }

  BOOST_TEST_MESSAGE( "\tTesting value encoding scheme." );



}
