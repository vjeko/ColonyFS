/*
 * chunkserver_harmony.h
 *
 *  Created on: Jun 27, 2008
 *      Author: vjeko
 */

#ifndef COM_CHUNKSERVER_HARMONY_HPP_
#define COM_CHUNKSERVER_HARMONY_HPP_

#include "server.hpp"
#include "../xmlrpc/harmony.hpp"
#include "../storage/db.hpp"
#include "../parsers/chunkserver_parser.hpp"

#include <boost/assign.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>

namespace uledfs {

namespace intercom {

class chunkserver_harmony: private server {
public:

  chunkserver_harmony(
			boost::asio::io_service&,
			uledfs::parser::chunkserver_parser&
			);

  void init();

	virtual ~chunkserver_harmony();

	virtual void process_cmd(
			boost::shared_ptr<command_t> command_ptr,
			const boost::system::error_code& e,
			connection_ptr_t conn);

	void deposit_chunk(connection_ptr_t);

  void display_data(
      const boost::system::error_code&,
      const boost::shared_ptr<uledfs::storage::chunk_data>,
      connection_ptr_t);

  void retrieve_chunk(
      connection_ptr_t);

  void send_data(
      const boost::system::error_code&,
      connection_ptr_t,
      const boost::shared_ptr<uledfs::storage::basic_metadata>);

  void handle_write(
      const boost::system::error_code& e,
      connection_ptr_t conn);

  static rlog::RLogChannel            *com_chunkserver_harmony_control_;

private:

  void end(const boost::system::error_code&, connection_ptr_t);

	/* Variable Declarations */
  uledfs::storage::db                 database_;
  uledfs::parser::chunkserver_parser& parser_;
  uledfs::xmlrpc::harmony             dht_;

};

} // namespace intercom

} // namespace uledfs

#endif /* CHUNKSERVER_H_ */
