/*
 * fuse_flush.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */

#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::flush(const char* filepath, struct fuse_file_info * fi) {
  data_sink_.flush();
  metadata_sink_.flush();

  return 0;
}
