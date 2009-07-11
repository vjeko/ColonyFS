/*
 * chunkserver_api.cpp
 *
 *  Created on: Mar 5, 2009
 *      Author: vjeko
 */

#include "chunkserver_api.hpp"

namespace uledfs {

chunkserver_api::chunkserver_api(
    boost::filesystem::path& config_path) :

      parser_(config_path),
      chunkserver_(io_service_, parser_) {

}

chunkserver_api::~chunkserver_api() { }

void chunkserver_api::run() {
  io_service_.run();
}

}
