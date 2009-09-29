/*
 * fuse_access.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */

#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::access(const char* filepath, int mode) {
  // TODO: Implement this.
    rLog(fuse_control_, "access: %s ", filepath);

  return 0;
}
