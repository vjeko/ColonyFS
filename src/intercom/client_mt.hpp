#ifndef COM_CLIENT_MT_HPP_
#define COM_CLIENT_MT_HPP_

#include "connection.hpp" // Must come before boost/serialization headers.
#include "intercom.hpp"
#include "../debug.hpp"

#include "../storage/chunk_data.hpp"

#include <iostream>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>

namespace uledfs {

namespace intercom {

class client_mt {

public:

  client_mt(boost::asio::io_service& io_service);

  virtual ~client_mt() = 0;

private:

  template <class Handler>
  void handle_connect(
      const boost::system::error_code& e,
      connection_ptr_t connection_ptr,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
      const command_t command,
      Handler handler) {

    if (!e) {

      //rLog(client_mt_control_, "Connection established with end-point %s.",
      //    connection_ptr->socket().remote_endpoint().address().to_string().c_str());

      connection_ptr->async_write(command,
          boost::bind( handler, connection_ptr) );

    } else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {

      connection_ptr->socket().close();
      boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

      connection_ptr->socket().async_connect(endpoint, boost::bind(
          &client_mt::handle_connect<Handler>, this,
          boost::asio::placeholders::error, connection_ptr,
          ++endpoint_iterator,
          command,
          handler)
      );

    } else {
      std::cerr << e.message() << std::endl;
    }
  }

  rlog::RLogChannel                  *client_mt_control_;

protected:

  template<class Handler>
  void init_connection(
      const std::string& hostname,
      const port_t port,
      const command_t command,
      Handler handler) {

    connection_ptr_t connection_ptr = connection_ptr_t(new connection(io_service_));

    boost::asio::ip::tcp::resolver resolver(connection_ptr->socket().io_service());
    boost::asio::ip::tcp::resolver::query query(
        hostname, boost::lexical_cast<std::string>(port));
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator;
    endpoint_iterator = resolver.resolve(query);

    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

    connection_ptr->socket().async_connect(endpoint,
        boost::bind(
            &client_mt::handle_connect<Handler>,
            this,
            boost::asio::placeholders::error, connection_ptr,
            ++endpoint_iterator,
            command, handler)
    );

  }

  boost::asio::io_service&            io_service_;
  static std::string                  service_;

};

}

}

#endif // COM_CLIENT_HPP
