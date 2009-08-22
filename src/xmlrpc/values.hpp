/*
 * base_value.hpp
 *
 *  Created on: Feb 3, 2009
 *      Author: vjeko
 */

#ifndef VALUES_HPP_
#define VALUES_HPP_

#include <string>
#include <vector>

#include <sys/stat.h>

#include <boost/exception.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include "attribute.hpp"




namespace uledfs { namespace xmlrpc {



typedef std::vector<std::string> chunkserver_list_t;



typedef boost::error_info<struct tag_size, int>  value_size;
class size_error : public boost::exception {};




enum instruction_enum {
  NOOP_INSTRUCTION,

  CHUNKSERVER_INSTRUCTION,
  FILENAME_INSTRUCTION,
  CHUNK_INSTRUCTION,
  ATTRIBUTE_INSTRUCTION
};




class instruction {

public:

  instruction(
      instruction_enum type = NOOP_INSTRUCTION,
      std::string argument = "") :
        type_(type), argument_(argument) {}


  instruction_enum get_type() {
    return type_;
  }

  std::string get_argument() {
    return argument_;
  }


private:
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
      ar & type_;
      ar & argument_;
  }

  instruction_enum  type_;
  std::string       argument_;
};



template<typename T>
class base_value {
public:

  typedef std::string   key_type;
  typedef T             value_type;
  typedef T             mapped_type;

  typedef boost::archive::binary_oarchive oarchive_type;
  typedef boost::archive::binary_iarchive iarchive_type;

  base_value() {};

  base_value(const instruction_enum type, const std::string argument = ""):
    instruction_(type, argument) {};

  base_value(const std::string& key, const std::string& value) {
    set_key(key);
    set_value(value);
  };

  virtual ~base_value() {};


  std::string&     get_key() {
    return base_value::serialize(instruction_, key_);
  };

  std::string&     get_value() {
    return base_value::serialize(mapped_, value_);
  }

  mapped_type&             get_mapped() {
    return mapped_;
  }

  const instruction&       get_instruction() {
    return instruction_;
  }

  void   set_key(const std::string& key) {
    deserialize(key, instruction_);
  };

  void   set_value(const std::string& value) {
    deserialize(value, mapped_);
  }

  template<typename Destination>
  void deserialize(const std::string& value, Destination& destionation) {
    std::stringstream ss(value);
    iarchive_type ia(ss);
    ia >> destionation;
  }

  template<typename Value>
  std::string& serialize(Value& value, std::string& destionation) {
    std::stringstream ss;
    oarchive_type oa(ss);
    oa << value;

    destionation = ss.str();
    return destionation;
  }

  instruction    instruction_;
  std::string    key_;

  T              mapped_;
  std::string    value_;
};





class chunkserver_value : public base_value< chunkserver_list_t > {

public:

  chunkserver_value(const std::string& key, const std::string& value) :
    base_value<chunkserver_list_t>(key, value) {};

  chunkserver_value(const std::string& swarm, const chunkserver_list_t& chunkservers) :
      base_value< chunkserver_list_t >(CHUNKSERVER_INSTRUCTION, swarm) {
    mapped_ = chunkservers;
  };

  virtual ~chunkserver_value() {};

  void append(std::string& hostname) {};
};





class filename_value : public base_value<size_t> {

public:

  filename_value(const std::string& key, const std::string& value) :
    base_value<size_t>(key, value) {};

  filename_value(const std::string& filename, const size_t chunk_num):
      base_value<size_t>(FILENAME_INSTRUCTION, filename) {
    mapped_ = chunk_num;
  };

  virtual ~filename_value() {};

};




class chunk_value : public base_value<std::string> {

public:

  chunk_value(const std::string& key, const std::string& value) :
    base_value<std::string>(key, value) {};

  chunk_value(
      const std::string& filename,
      const size_t chunk_num,
      const std::string& location) :
          base_value<std::string>(CHUNK_INSTRUCTION, boost::lexical_cast<std::string>(chunk_num) + filename) {
    mapped_ = location;
  };

  virtual ~chunk_value() {};
};




class attribute_value : public base_value<fattribute> {

public:

  attribute_value(const std::string& key, const std::string& value) :
    base_value<fattribute>(key, value) {};

  attribute_value(const std::string& filename) :
      base_value<fattribute>(ATTRIBUTE_INSTRUCTION, filename) {
  }

  attribute_value(const std::string& filename, const fattribute& a) :
      base_value<fattribute>(ATTRIBUTE_INSTRUCTION, filename) {
    mapped_ = a;
  }

  virtual ~attribute_value() {};
};

} }

#endif /* BASE_VALUE_HPP_ */
