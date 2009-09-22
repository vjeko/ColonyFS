/*
 * chunkserver_harmony.cpp
 *
 *  Created on: Jun 27, 2008
 *      Author: vjeko
 */


#include "chunkserver_harmony.hpp"
#include <boost/scoped_ptr.hpp>

#include "../storage/chunk_data.hpp"
#include "../storage/util.hpp"

namespace colony {

namespace intercom {

chunkserver_harmony::chunkserver_harmony(
    boost::asio::io_service& io_service,
    colony::parser::chunkserver_parser& parser) :
      server::server(io_service, CHUNK_SERVER_PORT),
      parser_(parser),
      dht_( parser_.get_dht_url() ) {

  this->init();

}

void chunkserver_harmony::init() {

  using xmlrpc::chunkserver_value;

  std::string                               hostname(boost::asio::ip::host_name());
  std::string                               swarm(parser_.get_swarm());
  chunkserver_value::value_type             chunkservers;

  boost::shared_ptr<chunkserver_value> chunkserver_info =
      ValueFactory<chunkserver_value>::NewPointer(swarm);

  try {

    dht_.get_pair(chunkserver_info);

  } catch (colony::xmlrpc::key_missing_error& e) {
    // If the swarm is missing, this implies we are the first one joining.
    rInfo("swarm seems to be empty... joining");

  }

  // Append our hostname to a given value...
  chunkserver_info->append(hostname);

  // ... and commit it to Harmony.
  dht_.set_pair(chunkserver_info);

  // Set the prefix for storing data.
  boost::filesystem::path path(parser_.get_path()[0]);
  colony::storage::set_prefix(path);

}

void chunkserver_harmony::process_cmd(
		boost::shared_ptr<command_t> command_ptr,
		const boost::system::error_code& e,
		connection_ptr_t conn) {

	if (!e) {

    rLog(com_chunkserver_harmony_control_, "received command %d", *command_ptr);

	  switch(*command_ptr) {
	  case DEPOSIT_CHUNK:
	    deposit_chunk(conn);
	    break;
	  case RETRIEVE_CHUNK:
	    retrieve_chunk(conn);
	    break;
	  case HEARTBEAT:
	    exit(1);
	    break;
	  default:
	    std::runtime_error("invalid command");
	  }

	} else {
		std::cerr << e.message() << std::endl;
	}
}

void chunkserver_harmony::deposit_chunk(connection_ptr_t conn) {

  boost::shared_ptr<colony::storage::chunk_data>
  chunk_ptr(new colony::storage::chunk_data);

  conn->async_read(
      *chunk_ptr,
      boost::bind(
          &chunkserver_harmony::display_data, this,
          boost::asio::placeholders::error,
          chunk_ptr, conn)
      );

  rLog(com_chunkserver_harmony_control_, "receiving chunk... ");
}


void chunkserver_harmony::display_data(
    const boost::system::error_code& e,
    const boost::shared_ptr<colony::storage::chunk_data> chunk_ptr,
    connection_ptr_t conn) {
  if (!e) {

    colony::storage::deposit_chunk(chunk_ptr, database_);

    rLog(com_chunkserver_harmony_control_, "done: (%s)(%d)(%lu)",
        chunk_ptr->uid_.c_str(), chunk_ptr->cuid_, chunk_ptr->data_ptr_->size());

  } else {
    std::cerr << e.message() << std::endl;
  }
}

void chunkserver_harmony::retrieve_chunk(
    connection_ptr_t conn) {

  boost::shared_ptr<colony::storage::basic_metadata>
  basic_metadata_ptr(new colony::storage::chunk_metadata );

  conn->async_read(
      *basic_metadata_ptr,
      boost::bind(
          &chunkserver_harmony::send_data, this,
          boost::asio::placeholders::error,
          conn, basic_metadata_ptr)
      );

  rLog(com_chunkserver_harmony_control_, "receiving chunk metadata...");
}

void chunkserver_harmony::send_data(
    const boost::system::error_code& e,
    connection_ptr_t conn,
    const boost::shared_ptr<colony::storage::basic_metadata> metadata_ptr) {

  if (!e) {

    boost::shared_ptr<colony::storage::chunk_data> chunk_ptr;
    chunk_ptr = colony::storage::retrieve_chunk(metadata_ptr, database_);

    std::cout << "POOP" << std::endl;

    rLog(com_chunkserver_harmony_control_, "sending data: (%s)(%d)(%lu)",
        chunk_ptr->uid_.c_str(), chunk_ptr->cuid_, chunk_ptr->data_ptr_->size());

    conn->async_write(*chunk_ptr,
        boost::bind(&chunkserver_harmony::end, this,
            boost::asio::placeholders::error,
            conn)
    );

  } else {
    std::cerr << e.message() << std::endl;
  }
}

void chunkserver_harmony::end(
    const boost::system::error_code& e,
    connection_ptr_t conn) {

  if (!e) {
    rLog(com_chunkserver_harmony_control_, "done... ");
  } else {
    std::cerr << e.message() << std::endl;
  }

}

void chunkserver_harmony::handle_write(
    const boost::system::error_code& e,
    connection_ptr_t conn) {

  if (!e) {
    rLog(com_chunkserver_harmony_control_, "done sending the data");
  } else {
    std::cerr << e.message() << std::endl;
  }


}

chunkserver_harmony::~chunkserver_harmony() {

}

rlog::RLogChannel *chunkserver_harmony::com_chunkserver_harmony_control_ = RLOG_CHANNEL( "intercom/chunkserver_harmony/control" );

}

}
