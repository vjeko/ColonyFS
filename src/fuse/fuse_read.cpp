/*
 * fuse_read.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::read(
    const char *filepath,
    char *buffer,
    size_t size,
    off_t offset,
    struct fuse_file_info *fi) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);

  rLog(fuse_control_, "read: %s", full.string().c_str());


  // Retrieve the attribute for the file path.
  shared_ptr<attribute_value> pair = metadata_sink_( full.string() );
  fattribute& attribute = pair->get_mapped();


  // TODO: Figure out size_t/off_t inconsistency.
  size_t length = attribute.stbuf.st_size;

  if (static_cast<size_t>(offset) > length) {

    size = 0;

  } else {

    if (static_cast<size_t>(offset) + size > length)
      size = length - offset;

    data_sink_.read(full, buffer, size, offset);

  }

  return size;


}
