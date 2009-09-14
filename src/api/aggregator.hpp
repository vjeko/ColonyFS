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
#include "../xmlrpc/harmony.hpp"

#include "../parsers/user_parser.hpp"

#include "../storage/bridge.hpp"
#include "../storage/basic_cache.hpp"

#include "../storage/chunk_data.hpp"
#include "../storage/chunk_metadata.hpp"

#include "../intercom/user_harmony.hpp"

#include <algorithm>
#include <sys/stat.h>

#include <boost/asio/io_service.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/exception.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>


namespace colony {

typedef boost::tuple<
      boost::asio::io_service,
      colony::intercom::user_harmony,
      colony::xmlrpc::harmony
    > bridge_type;


typedef boost::error_info<struct metadata_error, std::string> key_info_t;

class resource_error : public boost::exception {};



template<typename T>
class sink_local_impl {

public:

  typedef typename T::key_type           key_type;
  typedef typename T::value_type         value_type;
  typedef typename T::mapped_type        mapped_type;

  typedef boost::unordered_map<std::string, std::string>  implementation_type;
  typedef typename implementation_type::iterator          iterator;
  typedef typename implementation_type::const_iterator    const_iterator;




  sink_local_impl() {};




  virtual ~sink_local_impl() {};




  inline shared_ptr<T> operator()(const key_type& key) {
    return cache_.read(T::get_signature(key));
  }




  inline void commit(shared_ptr<T> pair) {
    shared_ptr<T> hit = cache_.mutate(pair->get_key());
    (*hit) = (*pair);
  }




  inline void erase(shared_ptr<T> object) {
    //implementation_.erase(object->get_key());
  }




  inline void erase(const key_type& key) {
    //T object(key);
    //implementation_.erase(object.get_key());
  }




private:



  basic_cache<T>       cache_;
};






template<typename T>
class sink_remote_impl {

public:

  typedef typename T::key_type           key_type;
  typedef typename T::value_type         value_type;
  typedef typename T::mapped_type        mapped_type;

  typedef boost::unordered_map<std::string, std::string>  implementation_type;
  typedef typename implementation_type::iterator          iterator;
  typedef typename implementation_type::const_iterator    const_iterator;




  sink_remote_impl() :
    dht_("http://barb.cs.washington.edu:36459") {

  };




  virtual ~sink_remote_impl() {};




  inline shared_ptr<T> operator()(const key_type& key) {
    shared_ptr<T> pair = make_shared<T>(key);
    typename implementation_type::iterator it = implementation_.find(pair->get_key());

    if(it == implementation_.end())
      throw colony::lookup_e();

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

  colony::xmlrpc::harmony        dht_;
  boost::asio::io_service        io_service_;
  std::vector<std::string>       chunkservers_;
  implementation_type            implementation_;
};




template<
  typename T,
  template <typename T> class Implementation = sink_local_impl
> class aggregator {

public:

  typedef typename T::key_type           key_type;
  typedef typename T::value_type         value_type;
  typedef typename T::mapped_type        mapped_type;




  aggregator() {};




  virtual ~aggregator() {};




  inline shared_ptr<T> operator()(const key_type& key) {
    return implementation_(key);
  }




  inline void commit(shared_ptr<T> pair) {
    implementation_.commit(pair);
  }




  inline void erase(shared_ptr<T> object) {
    implementation_.erase(object);
  }




  inline void erase(const key_type& key) {
    implementation_.erase(key);
  }




private:



  boost::asio::io_service             io_service_;
  Implementation<T>                   implementation_;
};



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Data
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



template<> class aggregator<colony::storage::chunk_data> {

public:


  aggregator() :
      bridge_(io_service_),
      client_( *(bridge_.client_) ),
      dht_( *(bridge_.dht_) ),
      sink_log_( RLOG_CHANNEL( "sink/data" ) ) {

    const std::string& swarm = bridge_.parser_->get_swarm();
    colony::xmlrpc::harmony& dht = *(bridge_.dht_);

    try {

      xmlrpc::chunkserver_value chunkserver_info =
          dht.get_value<xmlrpc::chunkserver_value>(swarm);

      chunkservers_ = chunkserver_info.get_mapped();

      rError("found %lu chunkservers", chunkservers_.size());

    } catch (colony::xmlrpc::key_missing_error& e) {
      rError("no chunkservers found...");
    }

  }


  void write(
      const boost::filesystem::path filepath,
      const char* buffer,
      const size_t size,
      const size_t offset) {

    using colony::storage::chunk_data;

    copy(
        filepath,
        buffer,
        boost::bind< shared_ptr<chunk_data> >(&aggregator::create_if_missing, this, _1, _2),
        boost::bind(&aggregator::write_to_chunk, this, _1, _2, _3, _4, _5),
        size,
        offset);

  }




  void read(
      const boost::filesystem::path filepath,
      char* buffer,
      const size_t size,
      const size_t offset) {

    using colony::storage::chunk_data;

    copy(
        filepath,
        buffer,
        boost::bind< shared_ptr<chunk_data> >(&aggregator::raise_if_missing, this, _1, _2),
        boost::bind(&aggregator::read_from_chunk, this, _1, _2, _3, _4, _5),
        size,
        offset);

  }




  void truncate(
      const boost::filesystem::path filepath,
      const size_t new_size,
      const size_t old_size) {

    using colony::storage::chunk_data;


    const size_t chunk_index_start = new_size / CHUNK_SIZE;
    const size_t chunk_index_end = old_size / CHUNK_SIZE;

    shared_ptr<chunk_data> chunk = raise_if_missing(filepath, chunk_index_start);

    const size_t chunk_size = new_size % CHUNK_SIZE;
    chunk_data::data_type& chunk_buffer = *(chunk->data_ptr_);
    chunk_buffer.resize(chunk_size);

    for (size_t count = chunk_index_start + 1; count <= chunk_index_end; count++) {
      const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);
      //implementation_.erase(key);
    }

  }




  void rename(
      const boost::filesystem::path oldpath,
      const boost::filesystem::path newpath,
      size_t total_size) {

    using colony::storage::chunk_data;

    const size_t chunk_index_start = 0;
    const size_t chunk_index_end = total_size / CHUNK_SIZE;

    for (size_t count = chunk_index_start; count <= chunk_index_end; count++) {
      const std::string old_key = oldpath.string() + boost::lexical_cast<std::string>(count);
      const std::string new_key = newpath.string() + boost::lexical_cast<std::string>(count);
      //implementation_[new_key] = implementation_[old_key];
      //implementation_.erase(old_key);
    }

  }




  void erase(const boost::filesystem::path filepath, size_t total_size) {

    using colony::storage::chunk_data;

    const size_t chunk_index_start = 0;
    const size_t chunk_index_end = total_size / CHUNK_SIZE;

    for (size_t count = chunk_index_start; count <= chunk_index_end; count++) {
      const std::string key = filepath.string() + boost::lexical_cast<std::string>(count);
      //implementation_.erase(key);
    }

  }




private:




  template<
    typename Buffer,
    typename MissingKeyPolicy,
    typename ActionPolicy
  > void copy(
      const boost::filesystem::path filepath,
      Buffer buffer,
      MissingKeyPolicy key_policy,
      ActionPolicy action_policy,
      const size_t size,
      const size_t offset) {

    using colony::storage::chunk_data;


    size_t buffer_start = 0;
    size_t buffer_end = size;

    const size_t chunk_index_start = buffer_start / CHUNK_SIZE;
    const size_t chunk_index_end = buffer_end / CHUNK_SIZE;

    rLog(sink_log_, "---------------------------");
    rLog(sink_log_, "Buffer Offset: %lu", offset);
    rLog(sink_log_, "Buffer Size: %lu", size);
    rLog(sink_log_, "Chunk Index Start: %lu", chunk_index_start);
    rLog(sink_log_, "Chunk Index End: %lu", chunk_index_end);

    for (size_t count = chunk_index_start; count <= chunk_index_end; count++) {

      const size_t remaining_size = buffer_end - buffer_start;
      const size_t chunk_start = offset % CHUNK_SIZE;
      const size_t chunk_end = (buffer_start + remaining_size > CHUNK_SIZE) ? CHUNK_SIZE : chunk_start + remaining_size;
      const size_t chunk_delta = chunk_end - chunk_start;

      rLog(sink_log_, "Chunk Start: %lu", chunk_start);
      rLog(sink_log_, "Chunk End: %lu", chunk_end);
      rLog(sink_log_, "Remaining Size: %lu", remaining_size);
      rLog(sink_log_, "Chunk Delta: %lu", chunk_delta);
      rLog(sink_log_, "---------------------------");

      shared_ptr<chunk_data> chunk = key_policy(filepath, count);

      action_policy(chunk, buffer, chunk_start, buffer_start, chunk_delta);

      buffer_start += chunk_delta;
    }

  }




  void write_to_chunk(
      shared_ptr<colony::storage::chunk_data> destination,
      const char* source,
      size_t destination_offset,
      size_t source_offset,
      size_t chunk_delta
      ) {

    colony::storage::chunk_data::data_type& chunk_buffer = *(destination->data_ptr_);

    const size_t required_size = destination_offset + chunk_delta;
    if (chunk_buffer.size() < required_size) chunk_buffer.resize(required_size);

    memcpy(&chunk_buffer[destination_offset], source + source_offset, chunk_delta);

  }




  void read_from_chunk(
      shared_ptr<colony::storage::chunk_data> source,
      char* destination,
      size_t source_offset,
      size_t destination_offset,
      size_t chunk_delta
      ) {

    colony::storage::chunk_data::data_type& chunk_buffer = *(source->data_ptr_);
    memcpy(destination + destination_offset, &chunk_buffer[source_offset], chunk_delta);

  }




  shared_ptr<colony::storage::chunk_data> create_if_missing(
      const boost::filesystem::path filepath,
      const size_t count) {

    colony::storage::basic_metadata::key_type key(filepath.string(), count);

    return cache_.mutate(key);

  }




  shared_ptr<colony::storage::chunk_data> raise_if_missing(
      const boost::filesystem::path filepath,
      const size_t count) {

    colony::storage::basic_metadata::key_type key(filepath.string(), count);

    return cache_.read(key);

  }


  boost::asio::io_service                        io_service_;
  colony::bridge                                 bridge_;

  colony::intercom::user_harmony&                client_;
  colony::xmlrpc::harmony&                       dht_;

  basic_cache<colony::storage::chunk_data>       cache_;
  rlog::RLogChannel                             *sink_log_;
  std::vector<std::string>                       chunkservers_;
};




} // Namespace.
#endif /* AGGREGATOR_HPP_ */
