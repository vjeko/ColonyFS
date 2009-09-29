/*
 * fuse_create.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"



int colonyfs_fusexx::create(
    const char* filepath,
    mode_t mode,
    struct fuse_file_info* fi) {

  using namespace colony::xmlrpc;

  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  if (!validate_path(leaf)) return -ENAMETOOLONG;


  rLog(fuse_control_, "create: %s", full.string().c_str());
  Timer::Print();

  // Create the attribute for the new file.
  shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
  fattribute& attribute = pair->get_mapped();


  // Set the attribute.
  attribute.stbuf.st_mode = S_IFREG | mode; // File flags.
  attribute.stbuf.st_nlink = 1;
  attribute.stbuf.st_uid = getuid();
  attribute.stbuf.st_gid = getgid();
  attribute.stbuf.st_blksize = CHUNK_SIZE;
  clock_gettime(CLOCK_REALTIME, &attribute.stbuf.st_mtim); // Modification time.


  // Register the new file with its parent.
  shared_ptr<attribute_value> parent_pair = metadata_sink_[ branch.string() ];
  fattribute& parent_attribute = parent_pair->get_mapped();

  parent_attribute.list.push_back(leaf.string());


  // Initialize the data structure.
  data_sink_.write(full, 0, 0, 0);


  return 0;

}
