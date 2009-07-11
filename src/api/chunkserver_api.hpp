/*
 * chunkserver_api.hpp
 *
 *  Created on: Mar 5, 2009
 *      Author: vjeko
 */

#ifndef CHUNKSERVER_API_HPP_
#define CHUNKSERVER_API_HPP_

#include "../intercom/chunkserver_harmony.hpp"
#include "../parsers/chunkserver_parser.hpp"

#include <iostream>
#include <csignal>

#include <boost/asio/basic_io_object.hpp>

#include <boost/asio.hpp>

#include <rlog/StdioNode.h>

namespace uledfs {

class chunkserver_api {
public:
  chunkserver_api(boost::filesystem::path& config_path);
  virtual ~chunkserver_api();

  void run();

private:
  boost::asio::io_service                io_service_;
  uledfs::parser::chunkserver_parser     parser_;
  uledfs::intercom::chunkserver_harmony  chunkserver_;
};

}

#endif /* CHUNKSERVER_API_HPP_ */
