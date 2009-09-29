/*
 * fuse_utime.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"


int colonyfs_fusexx::utime(const char* filepath,  struct utimbuf* time) {

  using namespace colony::xmlrpc;


  const boost::filesystem::path full(filepath);

  rLog(fuse_control_, "utime: %s", full.string().c_str());


  // Update the time.
  shared_ptr<attribute_value> pair = metadata_sink_[ full.string() ];
  fattribute& attribute = pair->get_mapped();


  attribute.stbuf.st_atim.tv_sec = time->actime;
  attribute.stbuf.st_mtim.tv_sec = time->modtime;


  return 0;

}
