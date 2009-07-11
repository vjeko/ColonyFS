/*
 * base_value.cpp
 *
 *  Created on: Feb 3, 2009
 *      Author: vjeko
 */

#include "values.hpp"
#include "../debug.hpp"

#include <iostream>
#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

namespace uledfs {

namespace xmlrpc {

const unsigned int base_value::INSTRUCTION_SIZE = 8;
const unsigned int base_value::INSTRUCTION_ARG_SIZE = 32;
const unsigned int base_value::KEY_SIZE = INSTRUCTION_SIZE + INSTRUCTION_ARG_SIZE;
const unsigned int base_value::VALUE_SIZE = 376;
const unsigned int base_value::KEY_VALUE_SIZE = KEY_SIZE + VALUE_SIZE;

const char* base_value::NO_ARG = "";

const std::string base_value::FILL("_");

/* ++++++++++++++++++ BASE VALUE ++++++++++++++++++ */
/*
base_value::base_value(const pair_t& pair) :
	key_(pair.substr(0, KEY_SIZE)),
	value_(pair.substr(KEY_SIZE)) {

	// Make sure the value is not exceeding its maximum
	// size. Otherwise throw an exception.
	if (pair.size() > KEY_VALUE_SIZE) {
		throw size_error() << value_size(pair.size());
	}

	//boost::algorithm::trim(value_, std::locale("_"));

}
*/
base_value::base_value(const std::string key, const std::string value) {

  if (value.size() > VALUE_SIZE)
    throw size_error() << value_size(value.size());

  key_     = key;
  value_   = value;

}

base_value::base_value(
    const instruction_t instruction_name,
    const instruction_arg_t arg,
    const value_type value) {

	if (value.size() > VALUE_SIZE)
	  throw size_error() << value_size(value.size());

	key_    = get_signature(instruction_name, arg);
	value_  = value;

}

std::string base_value::get_signature(
    const instruction_t instruction_name,
    const instruction_arg_t arg) {

  if (instruction_name.size() > INSTRUCTION_SIZE)
    throw size_error() << value_size(instruction_name.size());

  if (arg.size() > INSTRUCTION_ARG_SIZE)
    throw size_error() << value_size(arg.size());

  instruction_t instruction(instruction_name);

  for (unsigned int i = 0; i < INSTRUCTION_SIZE - instruction_name.size(); i++) {
    instruction += FILL;
  }

  instruction_arg_t argument(arg);

  for (unsigned int i = 0; i < INSTRUCTION_ARG_SIZE - arg.size(); i++) {
    argument += FILL;
  }

  return instruction + argument;
}

base_value::base_value() {
	for (unsigned int i = 0; i < KEY_SIZE; i++) {
	  key_ += FILL;
	}
}

base_value::~base_value() {}

std::string& base_value::get_key() {
	return key_;
}

std::string& base_value::get_value() {
	return value_;
}

void base_value::set_value(std::string& value) {
  value_ = value;
}

void base_value::set_key(std::string& key) {
  key_ = key;
}

/* ++++++++++++++++++ CHUNKSERVER VALUE ++++++++++++++++++ */

chunkserver_value::chunkserver_value(const std::string& key, const std::string& value) :
    base_value::base_value(key, value) {

  // TODO: REMOVE THIS QUICK FIX!
  if (value_.empty()) return;

  boost::algorithm::split(chunkservers_, value_, boost::is_any_of(DELIMITER), boost::token_compress_on);;

  list_t::iterator it = chunkservers_.begin();
  while(it != chunkservers_.end()) {

    if (it->size() == 0) {
      chunkservers_.erase(it++);
      continue;
    } else {
      ++it;
    }
  }

}

chunkserver_value::chunkserver_value(
    const swarm_t& swarm,
    const list_t& chunkserver_list) :
      base_value::base_value(get_signature(swarm), "") {

  BOOST_FOREACH(const std::string& hostname, chunkserver_list) {
    value_ += DELIMITER + hostname;
  }

}

std::string chunkserver_value::get_signature(instruction_arg_t swarm) {
  return base_value::get_signature(_instruction_, swarm);
}

chunkserver_value::~chunkserver_value() { }

void chunkserver_value::append(std::string& hostname) {

  rInfo("there are %zd hosts in the swarm...", chunkservers_.size());

  list_t::iterator it = std::find(chunkservers_.begin(), chunkservers_.end(), hostname);

  if (it == chunkservers_.end()) {
    rInfo("we are not part of the swarm... joining");

    chunkservers_.push_back(hostname);
    value_ += DELIMITER + hostname;
  } else {
    rInfo("we are already part of the swarm");
  }

}

chunkserver_value::list_t& chunkserver_value::get_chunkservers() {
  return chunkservers_;
}

const std::string chunkserver_value::DELIMITER("|");
const char*       chunkserver_value::_instruction_ = "cs";

/* ++++++++++++++++++ FILENAME VALUE ++++++++++++++++++ */

filename_value::filename_value(const std::string& key, const std::string& value) :
  base_value::base_value(key, value) { }

filename_value::filename_value(
    const filename_t& filename,
    const index_t n) :

      base_value(_instruction_,
          filename,
          boost::lexical_cast<std::string>(n)) {

}

filename_value::~filename_value() {}

std::string filename_value::get_signature(key_type key) {
  return base_value::get_signature(_instruction_, key);
}


const char* filename_value::_instruction_ = "f";

/* ++++++++++++++++++ CHUNK VALUE ++++++++++++++++++ */

chunk_value::chunk_value(const std::string& key, const std::string& value) :
  base_value::base_value(key, value) { }

chunk_value::chunk_value(
    const filename_t& filename,
    const index_t n,
    const location_t& value) :

      base_value(_instruction_,
          boost::lexical_cast<instruction_arg_t>(n) + filename,
          value) {

}

chunk_value::~chunk_value() {}

std::string chunk_value::get_signature(key_type key) {
  return base_value::get_signature(_instruction_, key);
}

std::string chunk_value::get_signature(filename_t filename, index_t chunk_number) {
  return chunk_value::get_signature(boost::lexical_cast<std::string>(chunk_number) + filename);
}

const char* chunk_value::_instruction_ = "c";


/* ++++++++++++++++++ FILESYSTEM INTERFACE VALUE ++++++++++++++++++ */

attribute_value::attribute_value(const std::string& key, const std::string& value) :
  base_value::base_value(key, value) { }

attribute_value::attribute_value(const attribute_value::filename_t& filename, const fattribute& a)
    : base_value( _instruction_, filename) {
  value_ = serialize(a);
}

attribute_value::value_type attribute_value::get_mapped() {
  return deserialize(value_);
}

std::string attribute_value::serialize(const value_type& a) {

  std::ostringstream s;
  boost::archive::text_oarchive archive(s);
  archive << a;

  return s.str();
}

attribute_value::value_type attribute_value::deserialize(const std::string& value) {

  std::istringstream s(value);
  boost::archive::text_iarchive archive(s);

  fattribute a;
  archive >> a;

  return a;
}

attribute_value::~attribute_value() {}

const char* attribute_value::_instruction_ = "fs";


} }
