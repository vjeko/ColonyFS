/*
 * fuse_chmod.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"



int colonyfs_fusexx::chmod(const char* filename, mode_t mode) {

  using namespace colony::xmlrpc;


  rLog(fuse_control_, "chmod: %s", filename);

  int status;

  // Check User ID.
  shared_ptr<attribute_value> pair = metadata_sink_[ filename ];
  fattribute& attribute = pair->get_mapped();


  if ( attribute.stbuf.st_uid == getuid() || getuid() == 0 ) {
    attribute.stbuf.st_mode = mode;
    status = 0;
  } else {
    status = -EACCES;
  }


  return status;

}
