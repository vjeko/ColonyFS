/*
 * fuse_opendir.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */

#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::opendir (const char* filepath, struct fuse_file_info* fi) {
  // TODO: Implement this.
  rLog(fuse_control_, "opendir %s", filepath);

  return 0;
}
