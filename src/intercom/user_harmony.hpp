/*
 * masternode_mt.hpp
 *
 *  Created on: Jul 3, 2008
 *      Author: vjeko
 */

#ifndef USER_HPP_
#define USER_HPP_

#include "client_mt.hpp"
#include "../synchronization.hpp"

#include <boost/random.hpp>
#include <boost/bind/protect.hpp>

#include "../storage/network_metadata.hpp"
#include "../parsers/user_parser.hpp"

namespace colony {

namespace intercom {

class user_harmony : public client_mt {

public:


  user_harmony(
      boost::asio::io_service& io_service,
      const std::string swarm) :
        client_mt::client_mt(io_service),
        swarm_(swarm),
        user_harmony_control_( RLOG_CHANNEL( "intercom/user_harmony/control" ) ) {}


  virtual ~user_harmony() {}


  void retrieve_chunk(
      const std::string hostname,
      const boost::shared_ptr<colony::storage::chunk_data> data_ptr) {

    request_read(hostname, data_ptr);

  }


  void deposit_chunk(
      const std::string hostname,
      const boost::shared_ptr<colony::storage::chunk_data const> data_ptr) {

    get_metadata(hostname, data_ptr);

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
      const std::string hostname,
      const boost::shared_ptr<colony::storage::chunk_data> data_ptr) {

    rLog(user_harmony_control_, "chunk (%s)(%d) -> %s",
        data_ptr->uid_.c_str(), data_ptr->cuid_, hostname.c_str());

    boost::shared_ptr<colony::storage::network_metadata> network_metadata_ptr(
        new colony::storage::network_metadata(
            (*data_ptr).uid_,
            (*data_ptr).cuid_,
            hostname)
        );

    read_from_chunkserver(network_metadata_ptr, data_ptr);

  }


  void read_from_chunkserver(
      const boost::shared_ptr<colony::storage::network_metadata> network_metadata_ptr,
      const boost::shared_ptr<colony::storage::chunk_data> data_ptr) {

      boost::shared_ptr<colony::storage::basic_metadata> basic_metadata_ptr
      = boost::static_pointer_cast<colony::storage::basic_metadata>(network_metadata_ptr);

      chunkserver_init(network_metadata_ptr->host_, RETRIEVE_CHUNK,
          boost::protect(boost::bind(
              &user_harmony::send_metadata, this,
              _1, network_metadata_ptr, data_ptr))
      );

  }


  void send_metadata(
      connection_ptr_t connection_ptr,
      const boost::shared_ptr<colony::storage::basic_metadata> basic_metadata_ptr,
      const boost::shared_ptr<colony::storage::chunk_data> data_ptr) {

    rLog(user_harmony_control_, "sending chunk metadata (%s)(%d)",
        basic_metadata_ptr->uid_.c_str(), basic_metadata_ptr->cuid_);

    connection_ptr->async_write(*basic_metadata_ptr,
      boost::bind(&user_harmony::receive_data,
          this,
          connection_ptr,
          data_ptr)
    );

  }


  void receive_data(
      connection_ptr_t connection_ptr,
      const boost::shared_ptr<colony::storage::chunk_data> chunk_ptr) {

    rLog(user_harmony_control_, "receiving chunk data... ");

    connection_ptr->async_read(*chunk_ptr,
        boost::bind(&user_harmony::end_retrieve,
            this,
            boost::asio::placeholders::error,
            chunk_ptr,
            connection_ptr)
    );

  }



  /* void deposti_chunk(...) */



  void get_metadata(
      const std::string hostname,
      const boost::shared_ptr<colony::storage::chunk_data const> data_ptr) {

    write_to_chunkserver(hostname, data_ptr);

  }


  void write_to_chunkserver(
      const std::string& hostname,
      const boost::shared_ptr<colony::storage::chunk_data const> data_ptr) {

    chunkserver_init(hostname, DEPOSIT_CHUNK,
      boost::protect(boost::bind(
          &user_harmony::send_data,
          this,
          _1,
          data_ptr))
      );

  }


  void send_data(
      connection_ptr_t connection_ptr,
      const boost::shared_ptr<colony::storage::chunk_data const> chunk_ptr) {

    rLog(user_harmony_control_, "depositing chunk");

    connection_ptr->async_write(*chunk_ptr,
        boost::bind(&user_harmony::end_deposit,
            this,
            boost::asio::placeholders::error,
            chunk_ptr,
            connection_ptr)
    );

  }


  void end_deposit(
      const boost::system::error_code& e,
      const boost::shared_ptr<colony::storage::chunk_data const> data_ptr,
      connection_ptr_t connection_ptr) {

    rLog(user_harmony_control_, "... done (Size: %zu)" , data_ptr->data_ptr_->size());
  }


  void end_retrieve(
      const boost::system::error_code& e,
      const boost::shared_ptr<colony::storage::chunk_data const> data_ptr,
      connection_ptr_t connection_ptr) {

    rLog(user_harmony_control_, "... done (Size: %zu)", data_ptr->data_ptr_->size());
  }




  std::string                         swarm_;
  rlog::RLogChannel                  *user_harmony_control_;
};

}

}

#endif /* COM_MASTERNODE_HPP_ */
