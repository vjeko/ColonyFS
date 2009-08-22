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
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

#include "../../xmlrpc/values.hpp"



std::string g_filename("value_filename");
std::string g_hostname("codered.cs.washington.edu");
std::string g_swarm("swarm-name");
size_t g_chunk_num = 20;




BOOST_AUTO_TEST_CASE( value_filename ) {

  using namespace uledfs::xmlrpc;

  filename_value original(g_filename, g_chunk_num);

  std::string key = original.get_key();
  std::string value = original.get_value();

  filename_value reconstructed(key, value);
  instruction instruction = reconstructed.get_instruction();

  BOOST_CHECK_EQUAL(FILENAME_INSTRUCTION, instruction.get_type());
  BOOST_CHECK_EQUAL(g_filename, instruction.get_argument());
  BOOST_CHECK_EQUAL(g_chunk_num, reconstructed.get_mapped());
}



BOOST_AUTO_TEST_CASE( value_chunk ) {

  using namespace uledfs::xmlrpc;

  chunk_value original(g_filename, g_chunk_num, g_hostname);
  std::string key = original.get_key();
  std::string value = original.get_value();

  chunk_value reconstructed(key, value);
  instruction instruction = reconstructed.get_instruction();

  std::string argument = boost::lexical_cast<std::string>(g_chunk_num) + g_filename;

  BOOST_CHECK_EQUAL(CHUNK_INSTRUCTION, instruction.get_type());
  BOOST_CHECK_EQUAL(argument, instruction.get_argument());
  BOOST_CHECK_EQUAL(g_hostname, reconstructed.get_mapped());
}



BOOST_AUTO_TEST_CASE( value_chunkserver ) {

  using namespace uledfs::xmlrpc;

  chunkserver_value::value_type chunkservers;
  chunkservers.push_back("hostname1");
  chunkservers.push_back("hostname2");
  chunkservers.push_back("hostname3");

  chunkserver_value original(g_swarm, chunkservers);

  std::string key = original.get_key();
  std::string value = original.get_value();

  chunkserver_value reconstructed(key, value);
  instruction instruction = reconstructed.get_instruction();

  BOOST_CHECK_EQUAL(CHUNKSERVER_INSTRUCTION, instruction.get_type());
  BOOST_CHECK_EQUAL(g_swarm, instruction.get_argument());
  BOOST_CHECK(chunkservers == reconstructed.get_mapped());

  BOOST_TEST_MESSAGE( "\tTesting value encoding scheme." );
}
