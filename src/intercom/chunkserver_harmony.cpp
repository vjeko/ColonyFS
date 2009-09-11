/*
 * chunkserver_harmony.cpp
 *
 *  Created on: Jun 27, 2008
 *      Author: vjeko
 */


#include "chunkserver_harmony.hpp"

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

  std::string                               hostname(boost::asio::ip::host_name());
  std::string                               swarm(parser_.get_swarm());
  xmlrpc::chunkserver_value::value_type     chunkservers;

  xmlrpc::chunkserver_value chunkserver_info(swarm, chunkservers);

  try {

    chunkserver_info = dht_.get_value<xmlrpc::chunkserver_value>(chunkserver_info.get_key());

    // See if the swarm exists. If so, a proper key is returned.
    //chunkserver_info = dht_.get_value<xmlrpc::chunkserver_value>(swarm);
  } catch (colony::xmlrpc::key_missing_error& e) {
    // If the swarm is missing, this implies we are the first one joining.
    rInfo("swarm seems to be empty... joining");
  }

  // Append our hostname to a given value...
  chunkserver_info.append(hostname);

  foreach(std::string& host, chunkserver_info.get_mapped()) {
    rInfo("Host: %s", host.c_str());
  }

  // ... and commit it to Harmony.
  dht_.put(chunkserver_info.get_key(), chunkserver_info.get_value());

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

    rLog(com_chunkserver_harmony_control_, "... (%s)(%d)(...)",
        chunk_ptr->uid_.c_str(), chunk_ptr->cuid_);

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

    rLog(com_chunkserver_harmony_control_, "... done: (%s)(%d)",
        metadata_ptr->uid_.c_str(), metadata_ptr->cuid_);

    boost::shared_ptr<colony::storage::chunk_data> chunk_ptr;
    chunk_ptr = colony::storage::retrieve_chunk(metadata_ptr, database_);

    conn->async_write(*chunk_ptr,
        boost::bind(&chunkserver_harmony::end, this,
            boost::asio::placeholders::error,
            conn)
    );

    rLog(com_chunkserver_harmony_control_, "sending chunk data... ");

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
    rLog(com_chunkserver_harmony_control_, "Done sending the data.");
  } else {
    std::cerr << e.message() << std::endl;
  }


}

chunkserver_harmony::~chunkserver_harmony() {

}

rlog::RLogChannel *chunkserver_harmony::com_chunkserver_harmony_control_ = RLOG_CHANNEL( "intercom/chunkserver_harmony/control" );

}

}
