/*
 * fuse_getattr.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::getattr(const char *filepath, struct stat *stat) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);
  const boost::filesystem::path branch = full.parent_path();
  const boost::filesystem::path leaf = full.filename();

  if (!validate_path(leaf)) return -ENAMETOOLONG;

  try {

    // Retrieve the attribute for the file path.
    shared_ptr<attribute_value> pair = metadata_sink_( full.string() );
    const fattribute& attribute = pair->get_mapped();


    // Double check that the returned attribute is valid.
    BOOST_ASSERT(
        (attribute.stbuf.st_nlink != 0) ||
        S_ISLNK(attribute.stbuf.st_mode)
        );


    memset(stat, 0, sizeof(struct stat));
    memcpy(stat, &attribute.stbuf, sizeof(struct stat));

    rLog(fuse_control_, "getattr: (+) %s", full.string().c_str());

    return 0;

  } catch (colony::lookup_e& e) {

    rLog(fuse_control_, "getattr: (-) %s", full.string().c_str());

    return -ENOENT;

  }

}
