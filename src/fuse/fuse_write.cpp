/*
 * fuse_write.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::write (
    const char* filepath,
    const char* buffer,
    size_t size,
    off_t offset,
    struct fuse_file_info* fi) {

  using namespace colony::xmlrpc;


  //rLog(fuse_control_, "write: (%lu) %s", size, filepath);

  boost::filesystem::path full(filepath);

  data_sink_.write(full, buffer, size, offset);

  // Update the attribute.
  shared_ptr<attribute_value> pair = metadata_sink_[ filepath ];
  fattribute& metadata = pair->get_mapped();

  const int end_pointer = offset + size;
  if (metadata.stbuf.st_size < end_pointer) metadata.stbuf.st_size = end_pointer;


  return size;

}
