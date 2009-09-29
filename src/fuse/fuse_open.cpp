/*
 * fuse_open.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"



int colonyfs_fusexx::open(const char *filepath, struct fuse_file_info *fi) {
  // TODO: Need to check if the operation is permitted for the given flags.
  rLog(fuse_control_, "open: %s (%d)", filepath, fi->flags);

  Timer::Print();

  return 0;
}
