/*
 * masternode_mt.hpp
 *
 *  Created on: Jul 3, 2008
 *      Author: vjeko
 */

#ifndef USER_HPP_
#define USER_HPP_

#include "client_mt.hpp"

#include <boost/random.hpp>

#include "../xmlrpc/values.hpp"
#include "../xmlrpc/harmony.hpp"

#include "../storage/network_metadata.hpp"
#include "../parsers/user_parser.hpp"

namespace uledfs {

namespace intercom {

class user_harmony : public client_mt {

public:

  user_harmony(
      boost::asio::io_service& io_service,
      uledfs::parser::user_parser& parser,
      uledfs::xmlrpc::harmony& dht);

  virtual ~user_harmony();

  void retrieve_chunk(
      boost::shared_ptr<uledfs::storage::chunk_data>) ;

  void deposit_chunk(
      boost::shared_ptr<uledfs::storage::chunk_data const>);

  /*
   * Retrieve a random value from a container.
   *
   * Requires: Forward Iterator.
   */
  template<typename T>
  typename T::value_type random_value(T& container) {

    // Initialize the random number generator.
    boost::uniform_int<> range(0, container.size() - 1);
    boost::variate_generator<rng_type&, boost::uniform_int<> > generator(rng_, range);

    // Generate secret number.
    const size_t index   = generator();

    typename T::const_iterator it = container.begin();

    for (size_t i = 0; i < index; i++) {  ++it; }
    return (*it);
  }

private:

  typedef boost::mt19937 rng_type;

  template<class Handler>
  void masternode_init(
      const std::string& hostname,
      const command_t command,
      Handler handler) {

    init_connection(hostname, MASTERNODE_PORT, command, handler);

  }

  template<class Handler>
  void chunkserver_init(
      const std::string& hostname,
      const command_t command,
      Handler handler) {

    init_connection(hostname, CHUNK_SERVER_PORT, command, handler);

  }

  /* void retrieve_chunk(...); */

  void request_read(
      boost::shared_ptr<uledfs::storage::chunk_data>);

  void read_from_chunkserver(
      boost::shared_ptr<uledfs::storage::network_metadata>,
      boost::shared_ptr<uledfs::storage::chunk_data>);

  void send_metadata(
      connection_ptr_t,
      boost::shared_ptr<uledfs::storage::basic_metadata>,
      boost::shared_ptr<uledfs::storage::chunk_data>);

  void receive_data(
      connection_ptr_t,
      boost::shared_ptr<uledfs::storage::chunk_data>);

  /* void deposti_chunk(...) */

  void get_metadata(
      boost::shared_ptr<uledfs::storage::chunk_data const>);

  void write_to_chunkserver(
      const std::string& hostname,
      boost::shared_ptr<uledfs::storage::chunk_data const>);

  void send_data(
      connection_ptr_t,
      boost::shared_ptr<uledfs::storage::chunk_data const>);

  void end(
      const boost::system::error_code& e,
      connection_ptr_t);

  void end(
      const boost::system::error_code& e,
      const boost::shared_ptr<uledfs::storage::chunk_data const> data_ptr,
      connection_ptr_t connection_ptr);

  uledfs::parser::user_parser        &parser_;
  uledfs::xmlrpc::harmony            &dht_;
  rlog::RLogChannel                  *user_harmony_control_;

  rng_type                                  rng_;
  xmlrpc::chunkserver_value::list_t chunkservers_;
};

}

}

#endif /* COM_MASTERNODE_HPP_ */
