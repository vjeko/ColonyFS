#ifndef COM_SERVER_HPP_
#define COM_SERVER_HPP_

// connection.hpp must come before boost/serialization headers.
#include "connection.hpp"
#include "intercom.hpp"
#include "../debug.hpp"

#include <iostream>
#include <vector>

// A compiler is not required to provide a header
// <climits> as a readable text file, but I do not
// know of any compilers which do not.
#include <climits>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>

namespace uledfs {

namespace intercom {

class server : boost::noncopyable {
public:

	server(boost::asio::io_service&, const port_t);

	/*
	 * Pure virtual functions result in abstract classes
	 * that can't be instantiated.
	 */
	virtual ~server() = 0;

private:

	virtual void handle_accept(
			const boost::system::error_code& e,
			connection_ptr_t conn) ;

  virtual void process_cmd(
      const boost::shared_ptr<command_t> command_ptr,
      const boost::system::error_code& e,
      connection_ptr_t conn) = 0;

	const port_t                   port_;
	boost::asio::ip::tcp::acceptor acceptor_;
	rlog::RLogChannel             *com_server_control_;

};

}

}

#endif
