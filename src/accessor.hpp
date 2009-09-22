/*
 * accessor.hpp
 *
 *  Created on: Sep 21, 2009
 *      Author: vjeko
 */

#ifndef ACCESSOR_HPP_
#define ACCESSOR_HPP_

#include <boost/asio/io_service.hpp>

#include "xmlrpc/harmony.hpp"

class DHT {

public:

  typedef colony::xmlrpc::harmony instance_type;

  static instance_type& Instance() {
    static instance_type dht("harmony-test");
    return dht;
  }

private:

  DHT();

};




class Client {

public:

  typedef colony::intercom::user_harmony instance_type;
  typedef boost::asio::io_service        io_service_type;

  static io_service_type& IoService() {
    static io_service_type io_service;
    return io_service;
  }

  static instance_type& Instance() {
    static instance_type client(IoService(), "harmony-test");
    return client;
  }

private:

  Client();

};



#endif /* ACCESSOR_HPP_ */
