/*
 * fuse_fgetattr.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */

#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::fgetattr (
    const char* filename,
    struct stat* stbuf,
    struct fuse_file_info* fi) {

  return getattr(filename, stbuf);

}
