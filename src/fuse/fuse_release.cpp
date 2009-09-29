/*
 * fuse_release.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::release(const char* filepath, struct fuse_file_info* fi) {
  // TODO: Implement this.
  rLog(fuse_control_, "release: %s (%d)", filepath, fi->flags);

  return 0;
}
