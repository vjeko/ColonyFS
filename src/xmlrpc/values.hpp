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

/**
 *
 * encoding type: fixed width
 * maximum size:  1024 bytes
 * value layout:
 *
 * | VALUE                            |
 * | 136 bytes            | 376 bytes |
 * | instruction encoding | value     |
 *
 * instruction encoding layout:
 *
 * | INSTRUCTION ENCODING |
 * | 8 bytes | 128 bytes  |
 * | type    | subtype    |
 *
 */
namespace uledfs { namespace xmlrpc {

typedef boost::error_info<struct tag_size, int>  value_size;

class size_error : public boost::exception {};

class base_value {
public:

  typedef std::string  instruction_t;
  typedef std::string  instruction_arg_t;

  typedef std::string  key_type;
  typedef std::string  value_type;

	base_value();
	base_value(const instruction_t, const instruction_arg_t, const value_type);
  base_value(const std::string key, const std::string value);
	virtual ~base_value();

	virtual value_type&     get_key();
	virtual value_type&     get_value();

	virtual void       set_key(key_type&);
	virtual void       set_value(value_type&);

	static std::string get_signature(
	    const instruction_t intruction,
	    const instruction_arg_t type);

  static const unsigned int VALUE_SIZE;
  static const unsigned int INSTRUCTION_SIZE;
  static const unsigned int INSTRUCTION_ARG_SIZE;
  static const unsigned int KEY_SIZE;
  static const unsigned int KEY_VALUE_SIZE;

  static const char* NO_ARG;

  static const std::string FILL;

protected:

  typedef const char*       signature_t;

	std::string    key_;
	std::string    value_;
};

class chunkserver_value : public base_value {
public:

  typedef std::string              swarm_t;
  typedef std::string              hostname_t;
  typedef std::vector<hostname_t>  list_t;

  typedef swarm_t                  key_type;
  typedef list_t                   value_type;

  chunkserver_value(const std::string& key, const std::string& value);
  chunkserver_value(const std::string& swarm, const list_t& chunkservers);

  template<typename K, typename V>
  chunkserver_value() {

  }

  virtual ~chunkserver_value();

  static std::string get_signature(swarm_t swarm);

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
  typedef std::string    filename_t;

  typedef filename_t     key_type;
  typedef index_t        value_type;

  filename_value(const std::string& key, const std::string& value);
  filename_value(const filename_t&, const index_t);
  virtual ~filename_value();

  static std::string get_signature(key_type key);

  static signature_t _instruction_;
};

class chunk_value : public base_value {
public:

  typedef int            index_t;
  typedef std::string    filename_t;
  typedef std::string    location_t;

  typedef std::string    key_type;
  typedef location_t     value_type;

	chunk_value(const std::string& key, const std::string& value);
	chunk_value(const filename_t&, const index_t, const location_t& location);
	virtual ~chunk_value();

	static std::string get_signature(key_type key);
	static std::string get_signature(filename_t filename, index_t chunk_number);

  static signature_t _instruction_;
};

class attribute_value : public base_value {

public:

  typedef std::string   filename_t;

  typedef filename_t     key_type;
  typedef fattribute     value_type;

  attribute_value(const std::string& key, const std::string& value);
  attribute_value(const filename_t&, const fattribute&);
  virtual ~attribute_value();

  value_type get_mapped();

  static std::string get_signature(key_t filename);
  static signature_t _instruction_;

private:

  std::string serialize(const value_type& attr);
  value_type deserialize(const std::string& value);
};

} }

#endif /* BASE_VALUE_HPP_ */
