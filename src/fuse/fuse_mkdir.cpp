/*
 * fuse_mkdir.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::mkdir(const char* filepath, mode_t mode) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  if (!validate_path(leaf)) return -ENAMETOOLONG;


  // TODO: Check if filename already exists.
  // Create the attribute for the new directory.
  shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
  fattribute& metadata = pair->get_mapped();


  memset(&metadata.stbuf, 0, sizeof(struct stat));
  metadata.stbuf.st_mode = S_IFDIR | mode;
  metadata.stbuf.st_nlink = 1;
  metadata.stbuf.st_uid = getuid();
  metadata.stbuf.st_gid = getgid();
  clock_gettime(CLOCK_REALTIME, &metadata.stbuf.st_mtim);



  rLog(fuse_control_, "mkdir: %s", full.string().c_str());


  // Retrieve the attribute associated with directory's parent.
  shared_ptr<attribute_value> parent_pair = metadata_sink_[ branch.string() ];
  fattribute& parent_attribute = parent_pair->get_mapped();


  parent_attribute.list.push_back(leaf.string());


  return 0;

}
