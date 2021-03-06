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
#include <iterator>


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
#include "../algo/ValueFactory.hpp"




namespace colony { namespace xmlrpc {




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


  instruction_enum get_type() const {
    return type_;
  }

  std::string get_argument() const {
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



template<typename T, typename Derived>
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

  base_value(const std::string key) {
    set_key(key);
  };

  virtual ~base_value() {};


  std::string&     get_key() {
    base_value::serialize(instruction_, key_);

    return key_;
  };

  std::string&     get_value() {
    base_value::serialize(mapped_, value_);

    return value_;
  }


  std::vector<unsigned char>     get_valuev() {
    return base_value::serialize(mapped_, value_byte_);
  }


  mapped_type&             get_mapped() {
    return mapped_;
  }

  instruction&       get_instruction() {
    return instruction_;
  }




  void   set_key(const std::string& key) {
    deserialize(key, instruction_);
  };

  void   set_value(const std::vector<unsigned char>& value) {
    deserialize(value, mapped_);
  }

  void   set_value(const std::string& value) {
    deserialize(value, mapped_);
  }




  void   set_mapped(const T mapped) {
    mapped_ = mapped;
  }

  void   set_instruction(const std::string arg) {
    instruction_ = instruction(Derived::signature_, arg);
  }


  template<typename Destination>
  static void deserialize(const std::vector<unsigned char>& value, Destination& destionation) {

    // TODO: Need a better implementation.
    std::string tmp(value.begin(), value.end());
    std::stringstream ss(tmp);

    iarchive_type ia(ss);
    ia >> destionation;

  }


  template<typename Destination>
  static void deserialize(const std::string& value, Destination& destionation) {
    std::stringstream ss(value);
    iarchive_type ia(ss);
    ia >> destionation;
  }


  template<typename Value>
  static std::vector<unsigned char>& serialize(Value& value, std::vector<unsigned char>& destionation) {

    // TODO: Need a better implementation.
    std::stringstream ss;
    oarchive_type oa(ss);
    oa << value;

    std::string tmp = ss.str();

    destionation = std::vector<unsigned char>(tmp.begin(), tmp.end());
    return destionation;
  }


  template<typename Value>
  static void serialize(Value& value, std::string& destionation) {
    std::stringstream ss;
    oarchive_type oa(ss);
    oa << value;

    destionation = ss.str();
  }

  static std::string get_signature(std::string argument) {
    instruction i(Derived::signature_, argument);

    std::string result;
    serialize(i, result);
    return result;
  }




  instruction    instruction_;
  std::string    key_;

  T              mapped_;
  std::string    value_;

  std::vector<unsigned char> value_byte_;
};





class chunkserver_value : public base_value< chunkserver_list_t, chunkserver_value > {

public:

  chunkserver_value() : base_value<chunkserver_list_t, chunkserver_value>() {}

  chunkserver_value(const std::string& swarm, const chunkserver_list_t& chunkservers) :
      base_value<chunkserver_list_t, chunkserver_value>(CHUNKSERVER_INSTRUCTION, swarm) {
    mapped_ = chunkservers;
  };

  void append(std::string& hostname) {
    mapped_.push_back(hostname);
  };

  const static instruction_enum signature_ = CHUNKSERVER_INSTRUCTION;
};


class filename_value : public base_value<size_t, filename_value> {

public:

  filename_value() : base_value<size_t, filename_value>() {}

  filename_value(const std::string& filename, const size_t chunk_num):
      base_value<size_t, filename_value>(FILENAME_INSTRUCTION, filename) {
    mapped_ = chunk_num;
  };

  virtual ~filename_value() {};

  const static instruction_enum signature_ = FILENAME_INSTRUCTION;

};




class chunk_value : public base_value<std::string, chunk_value> {

public:

  chunk_value() : base_value<std::string, chunk_value>() {}

  chunk_value(const std::string& key) :
      base_value<std::string, chunk_value>(signature_, key) {};

  chunk_value(
      const std::string& filename,
      const size_t chunk_num,
      const std::string& location) :
          base_value<std::string, chunk_value>(CHUNK_INSTRUCTION, boost::lexical_cast<std::string>(chunk_num) + filename) {
    mapped_ = location;
  };

  virtual ~chunk_value() {};

  const static instruction_enum signature_ = CHUNK_INSTRUCTION;
};




class attribute_value : public base_value<fattribute, attribute_value> {

public:

  attribute_value() : base_value<fattribute, attribute_value> () {}

  attribute_value(const std::string& filename) :
      base_value<fattribute, attribute_value>(ATTRIBUTE_INSTRUCTION, filename) {
  }

  attribute_value(const std::string& filename, const fattribute& a) :
      base_value<fattribute, attribute_value>(ATTRIBUTE_INSTRUCTION, filename) {
    mapped_ = a;
  }

  virtual ~attribute_value() {};

  const static instruction_enum signature_ = ATTRIBUTE_INSTRUCTION;
};


} }

#endif /* BASE_VALUE_HPP_ */
