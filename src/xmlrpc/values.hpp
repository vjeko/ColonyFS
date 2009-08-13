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
#include <list>

#include <sys/stat.h>

#include <boost/exception.hpp>

#include "attribute.hpp"




namespace uledfs { namespace xmlrpc {




typedef boost::error_info<struct tag_size, int>  value_size;
class size_error : public boost::exception {};




enum instruction_enum {
  NOOP,

  CHUNKSERVER_INSTRUCTION,
  FILENAME_INSTRUCTION,
  CHUNK_INSTRUCTION,
  ATTRIBUTE_INSTRUCTION
};




class instruction {

public:

  instruction(instruction_enum type = NOOP) :
      type_(type) {}

  instruction(instruction_enum type, std::string argument) :
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





class base_value {
public:

  base_value();
  base_value(const instruction_enum type, const std::string argument = "");
  base_value(const std::string key, const std::string value);
  virtual ~base_value();

  virtual std::string&     get_key();
  virtual std::string&     get_value();
  const instruction&       get_instruction();

  virtual void   set_key(const std::string&);
  virtual void   set_value(const std::string&);

  template<typename Value, typename Where>
  void deserialize(const Value& value, Where& where) {
    std::stringstream ss(value);
    boost::archive::text_iarchive ia(ss);
    ia >> where;
  }

  template<typename Value>
  std::string& serialize(Value& value) {
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << value;

    value_ = ss.str();
    return value_;
  }


protected:

  instruction    instruction_;
  std::string    key_;

  std::string    value_;
};




class chunkserver_value : public base_value {
public:

  typedef std::vector<std::string> chunkserver_list_t;
  typedef chunkserver_list_t       value_type;

  chunkserver_value(const std::string& key, const std::string& value);
  chunkserver_value(const std::string& swarm, const chunkserver_list_t& chunkservers);
  virtual ~chunkserver_value();

  void append(std::string& hostname);

  std::string& get_value();
  void set_value(const std::string& value);

  chunkserver_list_t& get_mapped();

protected:

  chunkserver_list_t chunkservers_;
};




class filename_value : public base_value {
public:

  filename_value(const std::string& key, const std::string& value);
  filename_value(const std::string& filename, const size_t chunk_num);
  virtual ~filename_value();


  size_t get_mapped();
  std::string& get_value();

protected:
  size_t chunk_num_;
};




class chunk_value : public base_value {

public:

  chunk_value(const std::string& key, const std::string& value);
  chunk_value(
      const std::string& filename,
      const size_t chunk_num,
      const std::string& location);

  virtual ~chunk_value();

  std::string& get_mapped();
};




class attribute_value : public base_value {

public:

  typedef std::string key_type;
  typedef fattribute  value_type;

  attribute_value(const std::string& key, const std::string& value);
  attribute_value(const std::string& filename, const fattribute& fattribute);
  virtual ~attribute_value();

  void         set_value(const std::string&);

  fattribute get_mapped();

  static std::string get_signature(std::string filename);

private:

  fattribute fattribute_;
};

} }

#endif /* BASE_VALUE_HPP_ */
