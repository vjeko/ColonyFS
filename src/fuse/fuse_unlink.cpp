/*
 * fuse_unlink.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"



int colonyfs_fusexx::unlink(const char* filepath) {


  using namespace colony::xmlrpc;


  rLog(fuse_control_, "unlink: %s", filepath);

  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  if (!validate_path(leaf)) return -ENAMETOOLONG;


  shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
  fattribute& attribute = pair->get_mapped();

  if ( attribute.stbuf.st_uid != getuid() && getuid() != 0 ) return -EACCES;

  // Remove the file from the parent.
  shared_ptr<attribute_value> parent_pair = metadata_sink_[ branch.string() ];
  fattribute& parent_attribute = parent_pair->get_mapped();

  fattribute::list_t& content = parent_attribute.list;
  content.remove(leaf.string());


  // Delete attribute.
  metadata_sink_.erase(full.string());

  // Delete the binary data.
  data_sink_.erase(full.string(), attribute.stbuf.st_size);


  return 0;

}
