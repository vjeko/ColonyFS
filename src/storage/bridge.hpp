/*
 * bridge.hpp
 *
 *  Created on: Sep 12, 2009
 *      Author: vjeko
 */

#ifndef BRIDGE_HPP_
#define BRIDGE_HPP_

#include "../xmlrpc/harmony.hpp"
#include "../parsers/user_parser.hpp"
#include "../intercom/user_harmony.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/filesystem/path.hpp>

namespace colony {

class bridge {

public:

  bridge(boost::asio::io_service& io_service) :
      parser_(new colony::parser::user_parser("conf/user.conf")),
      dht_(new colony::xmlrpc::harmony(parser_->get_swarm())),
      client_(new colony::intercom::user_harmony(io_service, parser_->get_swarm())) {

  }

  ~bridge() {}


  boost::shared_ptr<colony::parser::user_parser>     parser_;
  boost::shared_ptr<colony::xmlrpc::harmony>         dht_;
  boost::shared_ptr<colony::intercom::user_harmony>  client_;
};

}

#endif /* BRIDGE_HPP_ */
