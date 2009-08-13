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



base_value::base_value(const instruction_enum type, const std::string argument):
  instruction_(type, argument) {}



base_value::base_value(const std::string key, const std::string value) {
  set_key(key);
  set_value(value);
}



base_value::~base_value() {}



std::string& base_value::get_key() {
  std::stringstream ss;
  boost::archive::text_oarchive oa(ss);
  oa << instruction_;

  key_ = ss.str();
  return key_;
}



std::string& base_value::get_value() { return value_; }



void base_value::set_value(const std::string& value) {
  value_ = value;
}


void base_value::set_key(const std::string& key) {
  deserialize(key, instruction_);
}




/* ++++++++++++++++++ CHUNKSERVER VALUE ++++++++++++++++++ */




chunkserver_value::chunkserver_value(const std::string& key, const std::string& value) {}



chunkserver_value::chunkserver_value(
    const std::string& swarm,
    const chunkserver_list_t& chunkserver_list) :
      base_value(CHUNKSERVER, swarm),
      chunkservers_(chunkserver_list) {}


void chunkserver_value::set_value(const std::string& value) {
  std::stringstream oss(value);
  boost::archive::text_iarchive ia(oss);
  ia >> chunkservers_;
}



chunkserver_value::~chunkserver_value() { }



void chunkserver_value::append(std::string& hostname) {

  rInfo("there are %zd hosts in the swarm...", chunkservers_.size());

  chunkserver_list_t::iterator it =
      std::find(chunkservers_.begin(), chunkservers_.end(), hostname);

  if (it == chunkservers_.end()) {
    rInfo("we are not part of the swarm... joining");
    chunkservers_.push_back(hostname);
  } else {
    rInfo("we are already part of the swarm");
  }

}



chunkserver_value::chunkserver_list_t& chunkserver_value::get_mapped() {
  return chunkservers_;
}




/* ++++++++++++++++++ FILENAME VALUE ++++++++++++++++++ */




filename_value::filename_value(const std::string& key, const std::string& value) :
    base_value::base_value(key, value) {
  set_key(key);
  base_value::deserialize(value, chunk_num_);
}



filename_value::filename_value(
    const std::string& filename,
    const size_t chunk_num) :
      base_value(FILENAME, filename),
      chunk_num_(chunk_num) {}



filename_value::~filename_value() {}



size_t filename_value::get_mapped() {
  return chunk_num_;
}



std::string& filename_value::get_value() {
  return base_value::serialize(chunk_num_);
}




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




attribute_value::attribute_value(const std::string& key, const std::string& value) {
  set_key(key);
  set_value(value);
}



attribute_value::attribute_value(
    const std::string& filename,
    const fattribute& attribute)
    : base_value(ATTRIBUTE, filename),
      fattribute_(attribute) {}



void attribute_value::set_value(const std::string& value) {
  deserialize(value, fattribute_);
}



fattribute attribute_value::get_mapped() {
  return fattribute_;
}



attribute_value::~attribute_value() {}


} }
