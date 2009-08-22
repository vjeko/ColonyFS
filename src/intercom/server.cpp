#include "server.hpp"

#include <boost/thread.hpp>

namespace colony {

namespace intercom {

server::server(boost::asio::io_service& io_service, port_t port) :
		port_(port),
		acceptor_(
			io_service,
			boost::asio::ip::tcp::endpoint(
				boost::asio::ip::tcp::v4(),
				port_)
			),
		com_server_control_( RLOG_CHANNEL( "intercom/server/control" ) ) {

	rLog(com_server_control_, "listening on port %d.", port_);

	connection_ptr_t new_conn(new connection(acceptor_.io_service()));

	acceptor_.async_accept(
			new_conn->socket(),
			boost::bind(
					&server::handle_accept,
					this,
					boost::asio::placeholders::error,
					new_conn)
			);
}

server::~server() {};

void server::handle_accept(
		const boost::system::error_code& e,
		connection_ptr_t conn) {

	if (!e) {
		rLog(com_server_control_, "connection established with remote end-point %s:%d",
				conn->socket().remote_endpoint().address().to_string().c_str(),
				conn->socket().remote_endpoint().port());

		boost::shared_ptr<command_t> command_ptr(new command_t());

		conn->async_read(
				(*command_ptr),
				boost::bind(
						&server::process_cmd,
						this,
						command_ptr,
						boost::asio::placeholders::error,
						conn)
				);

		connection_ptr_t new_conn(new connection(acceptor_.io_service()));

		acceptor_.async_accept(
				new_conn->socket(),
				boost::bind(
						&server::handle_accept,
						this,
						boost::asio::placeholders::error,
						new_conn)
				);


	} else {
		std::cerr << e.message() << std::endl;
	}
}

}

}
