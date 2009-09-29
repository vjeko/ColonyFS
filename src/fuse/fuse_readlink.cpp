/*
 * fuse_readlink.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */

#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"



int colonyfs_fusexx::readlink (const char *linkname, char *buffer, size_t size) {

  using namespace colony::xmlrpc;


  // Retrieve the attribute associated with the link.
  shared_ptr<attribute_value> pair = metadata_sink_( linkname );
  fattribute& attribute = pair->get_mapped();


  // Is the file is a symbolic link?
  if (!S_ISLNK(attribute.stbuf.st_mode)) return -EINVAL;

  // Read the target where the link points to.
  const std::string targetpath = attribute.list.front();

  rLog(fuse_control_, "readlink: %s -> %s", linkname, targetpath.c_str());

  // We have to null-terminate the buffer!
  memset(buffer, NULL, size);
  memcpy(buffer, targetpath.c_str(), targetpath.size() + 1);
  // TODO: What if the linkname is too long to fit in the buffer?

  return 0;

}
