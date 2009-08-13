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

  CHUNKSERVER,
  FILENAME,
  CHUNK,
  ATTRIBUTE
};


class instruction {

public:

  instruction(instruction_enum type = NOOP) :
      type_(type) {}

  instruction(instruction_enum type, std::string subtype) :
      type_(type), subtype_(subtype) {}

private:
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
      ar & type_;
      ar & subtype_;
  }


  instruction_enum  type_;
  std::string       subtype_;
};

class base_value {
public:

	base_value();
	base_value(instruction_enum type, std::string subtype = "");
  base_value(const std::string key, const std::string value);
	virtual ~base_value();

	virtual std::string&     get_key();
	virtual std::string&     get_value();

	virtual void       set_key(std::string&);
	virtual void       set_value(std::string&);

protected:

  typedef const char*       signature_t;

  instruction    instruction_;
	std::string    key_;
	std::string    value_;
};

class chunkserver_value : public base_value {
public:

  typedef std::vector<std::string> list_t;
  typedef list_t                   value_type;

  chunkserver_value(const std::string& key, const std::string& value);
  chunkserver_value(const std::string& swarm, const list_t& chunkservers);
  virtual ~chunkserver_value();

  virtual std::string& get_value();
  virtual void         set_value(const std::string&);

  // Derived specific functions.
  void append(std::string& hostname);
  list_t& get_chunkservers();

  static signature_t        _instruction_;

protected:

  list_t                    chunkservers_;
  static const std::string  DELIMITER;
};

class filename_value : public base_value {
public:

  typedef int            index_t;

  filename_value(const std::string& key, const std::string& value);
  filename_value(const std::string& filename, const index_t);
  virtual ~filename_value();

  static signature_t _instruction_;
};

class chunk_value : public base_value {
public:

  typedef int            index_t;

	chunk_value(const std::string& key, const std::string& value);
	chunk_value(
	    const std::string& filename,
	    const index_t,
	    const std::string& location);

	virtual ~chunk_value();

  static signature_t _instruction_;
};

class attribute_value : public base_value {

public:

  typedef std::string key_type;
  typedef fattribute  value_type;

  attribute_value(const std::string& key, const std::string& value);
  attribute_value(const std::string& filename, const fattribute& fattribute);
  virtual ~attribute_value();

  virtual std::string& get_value();
  virtual void         set_value(const std::string&);

  fattribute get_mapped();

  static std::string get_signature(std::string filename);
  static signature_t _instruction_;

private:

  fattribute fattribute_;
};

} }

#endif /* BASE_VALUE_HPP_ */
