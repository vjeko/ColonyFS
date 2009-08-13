/*
 * masternode_mt.cpp
 *
 *  Created on: Jul 3, 2008
 *      Author: vjeko
 */

#include "user_harmony.hpp"

#include "../xmlrpc/values.hpp"

#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind/protect.hpp>

namespace uledfs { namespace intercom {

user_harmony::user_harmony(
    boost::asio::io_service& io_service,
    uledfs::parser::user_parser& parser,
    uledfs::xmlrpc::harmony& dht) :
      client_mt::client_mt(io_service),
      parser_(parser),
      dht_(dht),
      user_harmony_control_( RLOG_CHANNEL( "intercom/user_harmony/control" ) ) {


  xmlrpc::chunkserver_value chunkserver_value( dht_.get_value<xmlrpc::chunkserver_value>(parser_.get_swarm()) );
  chunkservers_ = chunkserver_value.get_mapped();

}

user_harmony::~user_harmony() { }

void user_harmony::retrieve_chunk(
    boost::shared_ptr<uledfs::storage::chunk_data> data_ptr) {

  rLog(user_harmony_control_, "retrieving chunk [%s][%d]",
      data_ptr->uid_.c_str(), data_ptr->cuid_);

  request_read(data_ptr);
}

void user_harmony::request_read(
    boost::shared_ptr<uledfs::storage::chunk_data> data_ptr) {

  std::string key(boost::lexical_cast<std::string>(data_ptr->cuid_) + data_ptr->uid_);

  xmlrpc::chunk_value chunk_info = dht_.get_value<xmlrpc::chunk_value>(key);

  rLog(user_harmony_control_, "chunk (%s)(%d) -> %s",
      data_ptr->uid_.c_str(), data_ptr->cuid_, chunk_info.get_value().c_str());;

  boost::shared_ptr<uledfs::storage::network_metadata> network_metadata_ptr(
      new uledfs::storage::network_metadata(
          (*data_ptr).uid_,
          (*data_ptr).cuid_,
          chunk_info.get_value())
      );

  read_from_chunkserver(network_metadata_ptr, data_ptr);

}

void user_harmony::read_from_chunkserver(
    boost::shared_ptr<uledfs::storage::network_metadata> network_metadata_ptr,
    boost::shared_ptr<uledfs::storage::chunk_data> data_ptr) {

    boost::shared_ptr<uledfs::storage::basic_metadata> basic_metadata_ptr
    = boost::static_pointer_cast<uledfs::storage::basic_metadata>(network_metadata_ptr);

    chunkserver_init(network_metadata_ptr->host_, RETRIEVE_CHUNK,
        boost::protect(boost::bind(
            &user_harmony::send_metadata, this,
            _1, network_metadata_ptr, data_ptr))
    );

}

void user_harmony::send_metadata(
    connection_ptr_t connection_ptr,
    boost::shared_ptr<uledfs::storage::basic_metadata> basic_metadata_ptr,
    boost::shared_ptr<uledfs::storage::chunk_data> data_ptr) {

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
    boost::shared_ptr<uledfs::storage::chunk_data> chunk_ptr) {

  rLog(user_harmony_control_, "receiving chunk data... ");

  connection_ptr->async_read(*chunk_ptr,
      boost::bind(&user_harmony::end,
          this,
          boost::asio::placeholders::error,
          chunk_ptr,
          connection_ptr)
  );

}

/* void deposti_chunk(...) */

void user_harmony::deposit_chunk(
    boost::shared_ptr<uledfs::storage::chunk_data const> data_ptr) {

  rLog(user_harmony_control_, "request for chunk mutation: <%s><%d>",
      data_ptr->uid_.c_str(), data_ptr->cuid_);

  get_metadata(data_ptr);

}

void user_harmony::get_metadata(
		boost::shared_ptr<uledfs::storage::chunk_data const> data_ptr) {

  const std::string chunkserver = random_value(chunkservers_);

  rLog(user_harmony_control_, "depositing chunk metadata <%s><%d><%s>",
      data_ptr->uid_.c_str(), data_ptr->cuid_, chunkserver.c_str() );

  xmlrpc::chunk_value chunk(
      data_ptr->uid_.c_str(),
      data_ptr->cuid_,
      chunkserver
      );

  dht_.put(chunk.get_key(), chunk.get_value());

  write_to_chunkserver(chunkserver, data_ptr);

}

void user_harmony::write_to_chunkserver(
    const std::string& hostname,
    boost::shared_ptr<uledfs::storage::chunk_data const> data_ptr) {

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
    boost::shared_ptr<uledfs::storage::chunk_data const> chunk_ptr) {

  rLog(user_harmony_control_, "depositing chunk");

  connection_ptr->async_write(*chunk_ptr,
      boost::bind(&user_harmony::end,
          this,
          boost::asio::placeholders::error,
          chunk_ptr,
          connection_ptr)
  );

}

void user_harmony::end(
    const boost::system::error_code& e,
    const boost::shared_ptr<uledfs::storage::chunk_data const> data_ptr,
    connection_ptr_t connection_ptr) {

  rLog(user_harmony_control_, "... done");

}

void user_harmony::end(
    const boost::system::error_code& e,
    connection_ptr_t connection_ptr) {

  rLog(user_harmony_control_, "... done");

}

} } // namespace
