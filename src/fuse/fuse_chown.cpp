/*
 * fuse_chown.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::chown(const char* filepath, uid_t uid, gid_t gid) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);

  rLog(fuse_control_, "chown: %s | UID: %ud GID: %ud", filepath, uid, gid);


  // Change user ID and group ID.
  shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
  fattribute& metadata = pair->get_mapped();


  metadata.stbuf.st_uid = uid;
  metadata.stbuf.st_gid = gid;


  return 0;

}
