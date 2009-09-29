/*
 * fuse_rmdir.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"



int colonyfs_fusexx::rmdir(const char* filepath) {

  using namespace colony::xmlrpc;

  rLog(fuse_control_, "rmdir: %s", filepath);

  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();


  // Remove metadata for the given file.
  metadata_sink_.erase(full.string().c_str());

  try {

    // Get the directory content of the parent.
    shared_ptr<attribute_value> parent_pair = metadata_sink_[ branch.string() ];
    fattribute& parent_metadata = parent_pair->get_mapped();


    // Examine the durectory content and remove the entry.
    parent_metadata.list.remove(leaf.string());

    return 0;

  } catch (colony::lookup_e& e) {

    rError("... metadata corruption -- parent directory not found");

    return -ENODEV;

  }

}
