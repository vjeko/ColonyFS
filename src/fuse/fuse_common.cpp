/*
 * fuse_common.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"



bool colonyfs_fusexx::validate_path(boost::filesystem::path path) {

  const size_t max_size = 255;
  bool result = true;

  if (path.string().length() > max_size) {
    result = false;
    std::cout << path.string().length() << "\t\tTOO LONG\n";
  }


  return result;

}
