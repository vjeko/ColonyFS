/*
 * aggregator.hpp
 *
 *  Created on: May 16, 2009
 *      Author: vjeko
 */

#ifndef AGGREGATOR_HPP_
#define AGGREGATOR_HPP_

#include "../debug.hpp"
#include "../xmlrpc/values.hpp"
#include "../xmlrpc/attribute.hpp"
#include "../storage/chunk_data.hpp"
#include "../storage/chunk_metadata.hpp"

#include <algorithm>
#include <iostream>
#include <sys/stat.h>

#include <boost/filesystem/path.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/exception.hpp>
#include <boost/lexical_cast.hpp>


namespace colony {




typedef boost::error_info<struct metadata_error, std::string> key_info_t;
class lookup_e : public boost::exception {};




template<typename T> class aggregator {

public:

  typedef typename T::key_type           key_type;
  typedef typename T::value_type         value_type;
  typedef typename T::mapped_type        mapped_type;

  typedef boost::unordered_map<std::string, std::string>  implementation_type;
  typedef typename implementation_type::iterator          iterator;
  typedef typename implementation_type::const_iterator    const_iterator;




  aggregator() {};




  virtual ~aggregator() {};




  inline shared_ptr<T> operator()(const key_type& key) {
    shared_ptr<T> pair = make_shared<T>(key);
    typename implementation_type::iterator it = implementation_.find(pair->get_key());

    if(it == implementation_.end())
      throw lookup_e() << key_info_t(pair->get_key());

    pair->set_value(it->second);

    return pair;
  }




  inline void commit(shared_ptr<T> pair) {
    implementation_[pair->get_key()] = pair->get_value();
  }




  inline void erase(shared_ptr<T> object) {
    implementation_.erase(object->get_key());
  }




  inline void erase(const key_type& key) {
    T object(key);
    implementation_.erase(object.get_key());
  }




private:

  size_t              num_;
  implementation_type implementation_;
};





template<> class aggregator<std::string> {


  typedef boost::unordered_map<std::string, shared_ptr<colony::storage::chunk_data> > implementation_type;


  void write(
      const boost::filesystem::path filepath,
      const char* buffer,
      const size_t offset,
      const size_t size) {

    using colony::storage::chunk_data;


    const size_t chunk_index_start = offset / CHUNK_SIZE;
    const size_t chunk_index_end = (offset + size) / CHUNK_SIZE;

    size_t pervious_pointer = 0;

    for (size_t count = chunk_index_start; count < chunk_index_end; count++) {

      const size_t chunk_start = std::max(offset, count * CHUNK_SIZE) % CHUNK_SIZE;
      const size_t chunk_end = std::min(offset + size, count * CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;

      const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);
      implementation_type::iterator it = implementation_.find(key);

      shared_ptr<chunk_data> chunk;

      if (it == implementation_.end()) {
        chunk = make_shared<chunk_data>(filepath.string(), count);
      } else {
        chunk = it->second;
      }

      chunk_data::data_type& data = *chunk->data_ptr_;

      memcpy(
          &data[chunk_start],
          buffer + pervious_pointer,
          chunk_end - chunk_start
          );

      pervious_pointer += chunk_end - chunk_start;

    }

  }

  void read(
      const boost::filesystem::path filepath,
      char* buffer,
      const size_t offset,
      const size_t size) {

    using colony::storage::chunk_data;


    const size_t chunk_index_start = offset / CHUNK_SIZE;
    const size_t chunk_index_end = (offset + size) / CHUNK_SIZE;

    size_t pervious_pointer = 0;

    for (size_t count = chunk_index_start; count < chunk_index_end; count++) {

      const size_t chunk_start = std::max(offset, count * CHUNK_SIZE) % CHUNK_SIZE;
      const size_t chunk_end = std::min(offset + size, count * CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;

      const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);
      implementation_type::iterator it = implementation_.find(key);

      shared_ptr<chunk_data> chunk;

      if (it == implementation_.end()) {
        std::runtime_error("missing chunk");
      } else {
        chunk = it->second;
      }

      chunk_data::data_type& data = *chunk->data_ptr_;

      memcpy(
          buffer + pervious_pointer,
          &data[chunk_start] ,
          chunk_end - chunk_start
          );

      pervious_pointer += chunk_end - chunk_start;

    }

  }



  void truncate(
      const boost::filesystem::path filepath,
      const size_t size) {

  }

  void erase(const boost::filesystem::path filepath) {

  }


private:

  implementation_type implementation_;
};




} // Namespace.
#endif /* AGGREGATOR_HPP_ */
