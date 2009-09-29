/*
 * fuse_readdir.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::readdir(
    const char* filepath, void* buffer,
    fuse_fill_dir_t filler, off_t offset,
    struct fuse_file_info* fi) {

  using namespace colony::xmlrpc;


  rLog(fuse_control_, "readdir: %s", filepath);

  boost::filesystem::path full(filepath);

  try {

    // Retrieve the attribute for the directory path.
    shared_ptr<attribute_value> pair = metadata_sink_( filepath );
    fattribute& attribute = pair->get_mapped();


    // Make sure we are dealing with the directory.
    if (!S_ISDIR(attribute.stbuf.st_mode)) {
      rError("... not a directory");
      return -ENOTDIR;
    }

    // Fill the buffer with the directory content.
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

    // And all other files.
    BOOST_FOREACH(const std::string& value, attribute.list) {

      rLog(fuse_control_, "*\t%s", value.c_str());

      // Pass the stat information as well!
      filler(buffer, value.c_str(),  NULL, 0);
    }

    return 0;

  } catch(colony::lookup_e& e) {

    rError("... directory not found");

    return -ENOENT;
  }

}
