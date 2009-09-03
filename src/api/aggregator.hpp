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
#include <sys/stat.h>

#include <boost/filesystem/path.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/exception.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/crc.hpp>


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





template<> class aggregator<colony::storage::chunk_data> {

public:

  typedef std::map<
    std::string,
    shared_ptr<colony::storage::chunk_data>
  > implementation_type;




  void write(
      const boost::filesystem::path filepath,
      const char* buffer,
      const size_t size,
      const size_t offset) {

    using colony::storage::chunk_data;

    std::cout << "size: " << size << std::endl;
    std::cout << "offset: " << offset << std::endl;

    size_t buffer_start = 0;
    size_t buffer_end = size;

    const size_t chunk_index_start = buffer_start / CHUNK_SIZE;
    const size_t chunk_index_end = buffer_end / CHUNK_SIZE;

    for (size_t count = chunk_index_start; count <= chunk_index_end; count++) {

      const size_t remaining_size = buffer_end - buffer_start;
      const size_t chunk_start = offset % CHUNK_SIZE;
      const size_t chunk_end = (offset + remaining_size > CHUNK_SIZE) ? CHUNK_SIZE : chunk_start + remaining_size;
      const size_t chunk_size = chunk_end - chunk_start;

      const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);

      shared_ptr<chunk_data> chunk = create_if_missing(filepath, count);

      chunk_data::data_type& raw_data = *(chunk->data_ptr_);
      raw_data.resize(chunk_end);
      copy_to_chunk(raw_data, buffer, chunk_start, buffer_start, chunk_size);


      buffer_start += chunk_size;
    }

  }




  void read(
      const boost::filesystem::path filepath,
      char* buffer,
      const size_t size,
      const size_t offset) {

    using colony::storage::chunk_data;

    std::cout << "size: " << size << std::endl;
    std::cout << "offset: " << offset << std::endl;

    size_t buffer_start = 0;
    size_t buffer_end = size;

    const size_t chunk_index_start = buffer_start / CHUNK_SIZE;
    const size_t chunk_index_end = buffer_end / CHUNK_SIZE;

    std::cout << "chunk start: " << chunk_index_start << std::endl;
    std::cout << "chunk end: " << chunk_index_end << std::endl;

    for (size_t count = chunk_index_start; count <= chunk_index_end; count++) {

      const size_t remaining_size = buffer_end - buffer_start;
      const size_t chunk_start = offset % CHUNK_SIZE;
      const size_t chunk_end = (offset + remaining_size > CHUNK_SIZE) ? CHUNK_SIZE : chunk_start + remaining_size;
      const size_t chunk_size = chunk_end - chunk_start;

      std::cout << "remaining_size: " << remaining_size << std::endl;
      std::cout << "chunk_start: " << chunk_start << std::endl;
      std::cout << "chunk_end: " << chunk_end << std::endl;
      std::cout << "chunk_size: " << chunk_size << std::endl;


      const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);

      shared_ptr<chunk_data> chunk = raise_if_missing(filepath, count);

      chunk_data::data_type& raw_data = *(chunk->data_ptr_);

      copy_from_chunk(raw_data, buffer, chunk_start, buffer_start, chunk_size);

      buffer_start += chunk_size;
    }

  }




  void truncate(
      const boost::filesystem::path filepath,
      const size_t new_size,
      const size_t old_size) {

    using colony::storage::chunk_data;


    const size_t chunk_index_start = new_size / CHUNK_SIZE;
    const size_t chunk_index_end = old_size / CHUNK_SIZE;

    const std::string key = filepath.string() + boost::lexical_cast<std::string>(chunk_index_start);

    shared_ptr<chunk_data> chunk;
    implementation_type::iterator it = implementation_.find(key);

    if (it == implementation_.end()) {
      std::runtime_error("unknown chunk");
    } else {
      chunk = it->second;
    }

    const size_t chunk_size = new_size % CHUNK_SIZE;
    chunk_data::data_type& raw_data = *(chunk->data_ptr_);
    raw_data.resize(chunk_size);

    for (size_t count = chunk_index_start + 1; count <= chunk_index_end; count++) {
      const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);
      implementation_.erase(key);
    }

  }




  void erase(const boost::filesystem::path filepath, size_t size) {

    using colony::storage::chunk_data;

    const size_t chunk_index_start = 0;
    const size_t chunk_index_end = size / CHUNK_SIZE;

    for (size_t count = chunk_index_start + 1; count <= chunk_index_end; count++) {
      const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);
      implementation_.erase(key);
    }

  }




private:


  void copy_to_chunk(
      colony::storage::chunk_data::data_type& destination,
      const char* source,
      size_t destination_offset,
      size_t source_offset,
      size_t size
      ) {

    destination.resize(CHUNK_SIZE);

    memcpy(
        &destination[destination_offset],
        source + source_offset,
        size
        );

  }




  void copy_from_chunk(
      colony::storage::chunk_data::data_type& source,
      char* destination,
      size_t source_offset,
      size_t destination_offset,
      size_t size
      ) {

    memcpy(
        destination,
        &source[source_offset],
        size
        );

  }




  shared_ptr<colony::storage::chunk_data> create_if_missing(
      const boost::filesystem::path filepath,
      const size_t count) {

    using colony::storage::chunk_data;

    const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);

    shared_ptr<chunk_data> chunk;
    implementation_type::iterator it = implementation_.find(key);

    if (it == implementation_.end()) {
      chunk = make_shared<chunk_data>(filepath.string(), count);
      implementation_[key] = chunk;
    } else {
      chunk = it->second;
    }

    return chunk;
  }




  shared_ptr<colony::storage::chunk_data> raise_if_missing(
      const boost::filesystem::path filepath,
      const size_t count) {

    const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);

    using colony::storage::chunk_data;

    shared_ptr<chunk_data> chunk;
    implementation_type::iterator it = implementation_.find(key);

    if (it == implementation_.end()) {
      std::runtime_error("unknown key");
    } else {
      chunk = it->second;
      std::cout << "CHUNK " << count << " |----> " << chunk->data_ptr_->size() << std::endl;
    }

    return chunk;
  }




  implementation_type implementation_;
};




} // Namespace.
#endif /* AGGREGATOR_HPP_ */
