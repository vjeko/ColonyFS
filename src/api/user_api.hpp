/*
 * client.hpp
 *
 *  Created on: Feb 21, 2009
 *      Author: vjeko
 */

#ifndef USER_API_HPP_
#define USER_API_HPP_

#include "../debug.hpp"                 // Debugging facility.
#include "../intercom/user_harmony.hpp" // Communication module.
#include "../storage/util.hpp"          // Storage module.
#include "../parsers/user_parser.hpp"   // Parser module.

#include <boost/asio/io_service.hpp>
#include <boost/filesystem/path.hpp>

namespace uledfs {

/**
 * Client's Public API.
 *
 * Provide rudimentary uleDFS IO calls.
 *
 * write(filename);
 * read(filename);
 *
 *
 */

class user_api {
public:

  typedef uledfs::parser::user_parser parser_t;

  user_api(
      parser_t& parser);

  virtual ~user_api();

  void write(boost::filesystem::path& in);

  void read(
      boost::filesystem::path& in,
      boost::filesystem::path& out);

private:

  boost::asio::io_service         io_service_;
  parser_t                       &parser_;
  uledfs::xmlrpc::harmony         dht_;
  uledfs::intercom::user_harmony  intercom_;
  rlog::RLogChannel*              user_log_;
};

}

#endif /* CLIENT_HPP_ */
