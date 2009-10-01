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

namespace colony {

namespace intercom {

class user_harmony : public client_mt {

public:

  user_harmony(
      boost::asio::io_service& io_service,
      std::string swarm);

  virtual ~user_harmony();

  void retrieve_chunk(
      std::string hostname,
      boost::shared_ptr<colony::storage::chunk_data>) ;

  void deposit_chunk(
      std::string hostname,
      boost::shared_ptr<colony::storage::chunk_data const>);



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
      std::string hostname,
      boost::shared_ptr<colony::storage::chunk_data>);

  void read_from_chunkserver(
      boost::shared_ptr<colony::storage::network_metadata>,
      boost::shared_ptr<colony::storage::chunk_data>);

  void send_metadata(
      connection_ptr_t,
      boost::shared_ptr<colony::storage::basic_metadata>,
      boost::shared_ptr<colony::storage::chunk_data>);

  void receive_data(
      connection_ptr_t,
      boost::shared_ptr<colony::storage::chunk_data>);

  /* void deposti_chunk(...) */

  void get_metadata(
      std::string hostname,
      boost::shared_ptr<colony::storage::chunk_data const>);

  void write_to_chunkserver(
      const std::string& hostname,
      boost::shared_ptr<colony::storage::chunk_data const>);

  void send_data(
      connection_ptr_t,
      boost::shared_ptr<colony::storage::chunk_data const>);

  void end_deposit(
      const boost::system::error_code& e,
      const boost::shared_ptr<colony::storage::chunk_data const> data_ptr,
      connection_ptr_t connection_ptr);

  void end_retrieve(
      const boost::system::error_code& e,
      const boost::shared_ptr<colony::storage::chunk_data const> data_ptr,
      connection_ptr_t connection_ptr);

  std::string                         swarm_;
  rlog::RLogChannel                  *user_harmony_control_;

  xmlrpc::chunkserver_value::value_type     chunkservers_;
};

}

}

#endif /* COM_MASTERNODE_HPP_ */
