/*
 * fuse_lock.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */

#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::lock (
    const char* filepath,
    struct fuse_file_info* fi,
    int cmd,
    struct flock* fl) {

  rError("Lock called: File: %s: CMD: %d", filepath, cmd);

  return 0;
}
