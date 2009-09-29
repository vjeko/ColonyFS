/*
 * fuse_truncate.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */

#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::truncate(const char* filepath, off_t length) {

  using namespace colony::xmlrpc;

  try {

    const boost::filesystem::path full(filepath);

    rLog(fuse_control_, "truncate: %s to %ld", filepath, length);

    // Is length negative?
    if (length < 0) return -EINVAL;

    // Modify the file size information.
    shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
    fattribute& attribute = pair->get_mapped();

    data_sink_.truncate(full, length, attribute.stbuf.st_size);

    attribute.stbuf.st_size = length;


    return 0;

  } catch (colony::lookup_e& e) {

    rError("... metadata corruption -- file not found");

    return -ENODEV;

  }


}
