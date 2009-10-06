/*
 * masternode_mt.cpp
 *
 *  Created on: Jul 3, 2008
 *      Author: vjeko
 */

#include "user_harmony.hpp"

#include "../accessor.hpp"
#include "../synchronization.hpp"


#include "../xmlrpc/values.hpp"

#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind/protect.hpp>

namespace colony { namespace intercom {

user_harmony::user_harmony(
    boost::asio::io_service& io_service,
    std::string swarm) :
      client_mt::client_mt(io_service),
      swarm_(swarm),
      user_harmony_control_( RLOG_CHANNEL( "intercom/user_harmony/control" ) ) {

}

user_harmony::~user_harmony() { }

void user_harmony::retrieve_chunk(
    std::string hostname,
    boost::shared_ptr<colony::storage::chunk_data> data_ptr) {

  //Sync::Lock(data_ptr->get_key());

  request_read(hostname, data_ptr);
}

void user_harmony::request_read(
    std::string hostname,
    boost::shared_ptr<colony::storage::chunk_data> data_ptr) {

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

void user_harmony::read_from_chunkserver(
    boost::shared_ptr<colony::storage::network_metadata> network_metadata_ptr,
    boost::shared_ptr<colony::storage::chunk_data> data_ptr) {

    boost::shared_ptr<colony::storage::basic_metadata> basic_metadata_ptr
    = boost::static_pointer_cast<colony::storage::basic_metadata>(network_metadata_ptr);

    chunkserver_init(network_metadata_ptr->host_, RETRIEVE_CHUNK,
        boost::protect(boost::bind(
            &user_harmony::send_metadata, this,
            _1, network_metadata_ptr, data_ptr))
    );

}

void user_harmony::send_metadata(
    connection_ptr_t connection_ptr,
    boost::shared_ptr<colony::storage::basic_metadata> basic_metadata_ptr,
    boost::shared_ptr<colony::storage::chunk_data> data_ptr) {

  rLog(user_harmony_control_, "sending chunk metadata (%s)(%d)",
      basic_metadata_ptr->uid_.c_str(), basic_metadata_ptr->cuid_);

  connection_ptr->async_write(*basic_metadata_ptr,
    boost::bind(&user_harmony::receive_data,
        this,
        connection_ptr,
        data_ptr)
  );
}

void user_harmony::receive_data(
    connection_ptr_t connection_ptr,
    boost::shared_ptr<colony::storage::chunk_data> chunk_ptr) {

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

void user_harmony::deposit_chunk(
    std::string hostname,
    boost::shared_ptr<colony::storage::chunk_data const> data_ptr) {

  Sync::Lock(data_ptr->get_key());

  get_metadata(hostname, data_ptr);

}

void user_harmony::get_metadata(
    std::string hostname,
		boost::shared_ptr<colony::storage::chunk_data const> data_ptr) {

  xmlrpc::chunk_value chunk(
      data_ptr->uid_.c_str(),
      data_ptr->cuid_,
      hostname
      );

  write_to_chunkserver(hostname, data_ptr);

}

void user_harmony::write_to_chunkserver(
    const std::string& hostname,
    boost::shared_ptr<colony::storage::chunk_data const> data_ptr) {

  chunkserver_init(hostname, DEPOSIT_CHUNK,
    boost::protect(boost::bind(
        &user_harmony::send_data,
        this,
        _1,
        data_ptr))
    );

}

void user_harmony::send_data(
    connection_ptr_t connection_ptr,
    boost::shared_ptr<colony::storage::chunk_data const> chunk_ptr) {

  rLog(user_harmony_control_, "depositing chunk");

  connection_ptr->async_write(*chunk_ptr,
      boost::bind(&user_harmony::end_deposit,
          this,
          boost::asio::placeholders::error,
          chunk_ptr,
          connection_ptr)
  );

}


void user_harmony::end_deposit(
    const boost::system::error_code& e,
    const boost::shared_ptr<colony::storage::chunk_data const> data_ptr,
    connection_ptr_t connection_ptr) {

  Sync::Unlock(data_ptr->get_key());

  rLog(user_harmony_control_, "... done (Size: %zu)" , data_ptr->data_ptr_->size());
  Timer::Print();

}

void user_harmony::end_retrieve(
    const boost::system::error_code& e,
    const boost::shared_ptr<colony::storage::chunk_data const> data_ptr,
    connection_ptr_t connection_ptr) {

  rLog(user_harmony_control_, "... done (Size: %zu)", data_ptr->data_ptr_->size());

	Sync::Unlock(data_ptr->get_key());

  Timer::Print();

}

} } // namespace
