/*
 * fuse_symlink.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */

#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"

int colonyfs_fusexx::symlink (const char* target, const char* link) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(link);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  rLog(fuse_control_, "symlink: %s -> %s", target, link);

  // Create the attribute what that be associated with the new link.
  //shared_ptr<attribute_value> pair = make_shared<attribute_value>(link);
  shared_ptr<attribute_value> pair = metadata_sink_[link];
  fattribute& attribute = pair->get_mapped();


  // Set the file mode for the new link.
  attribute.stbuf.st_mode = S_IFLNK | 0777;

  // What is the actual target for this link?
  attribute.list.push_back(target);

  // Set the modification time.
  clock_gettime(CLOCK_REALTIME, &attribute.stbuf.st_mtim); // Modification time.



  /* Register the link with its parent directory */

  // Retrieve the attribute associated with link's parent directory.
  shared_ptr<attribute_value> parent_pair = metadata_sink_[ branch.string() ];
  fattribute& parent_attribute = parent_pair->get_mapped();


  // Register the new link with the parent directory.
  parent_attribute.list.push_back(leaf.string());


  return 0;

}
