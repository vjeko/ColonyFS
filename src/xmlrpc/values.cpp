/*
 * base_value.cpp
 *
 *  Created on: Feb 3, 2009
 *      Author: vjeko
 */

#include "values.hpp"
#include "../debug.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

namespace uledfs {

namespace xmlrpc {

/* ++++++++++++++++++ BASE VALUE ++++++++++++++++++ */

base_value::base_value() {}

base_value::base_value(instruction_enum type, std::string subtype):
  instruction_(type) {}


base_value::base_value(const std::string key, const std::string value) {

  if (value.size() > 376)
    throw size_error() << value_size(value.size());

  key_     = key;
  value_   = value;
}

base_value::~base_value() {}

std::string& base_value::get_key() { return key_; }
std::string& base_value::get_value() { return value_; }

void base_value::set_value(std::string& value) { value_ = value; }
void base_value::set_key(std::string& key) { key_ = key; }

/* ++++++++++++++++++ CHUNKSERVER VALUE ++++++++++++++++++ */

chunkserver_value::chunkserver_value(const std::string& key, const std::string& value) {
  set_value(value);
}

chunkserver_value::chunkserver_value(
    const std::string& swarm,
    const list_t& chunkserver_list) :
      base_value(CHUNKSERVER),
      chunkservers_(chunkserver_list) {}

std::string& chunkserver_value::get_value() {

  std::stringstream oss;
  boost::archive::text_oarchive oa(oss);
  oa << chunkservers_;

  value_ = oss.str();
  return value_;
}

void chunkserver_value::set_value(const std::string& value) {
  std::stringstream oss(value);
  boost::archive::text_iarchive ia(oss);
  ia >> chunkservers_;
  value_ = value;
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
    const std::string& filename,
    const index_t n) :
      base_value(FILENAME, filename){

}

filename_value::~filename_value() {}

/* ++++++++++++++++++ CHUNK VALUE ++++++++++++++++++ */

chunk_value::chunk_value(const std::string& key, const std::string& value) :
  base_value::base_value(key, value) { }

chunk_value::chunk_value(
    const std::string& filename,
    const index_t n,
    const std::string& value) :
      base_value(CHUNK, boost::lexical_cast<std::string>(n) + filename) { }

chunk_value::~chunk_value() {}


/* ++++++++++++++++++ FILESYSTEM INTERFACE VALUE ++++++++++++++++++ */

attribute_value::attribute_value(const std::string& key, const std::string& value) {}

attribute_value::attribute_value(const std::string& filename, const fattribute& a)
    : base_value(ATTRIBUTE, filename),
      fattribute_(a){
}


fattribute attribute_value::get_mapped() {
  return fattribute_;
}


std::string& attribute_value::get_value() {
  std::stringstream ss;
  boost::archive::text_oarchive oa(ss);

  oa << fattribute_;
  value_ = ss.str();

  return value_;
}


void attribute_value::set_value(const std::string& value) {
  std::stringstream oss(value);
  boost::archive::text_iarchive ia(oss);

  ia >> fattribute_;
  value_ = value;
}

attribute_value::~attribute_value() {}

const char* attribute_value::_instruction_ = "fs";


} }
