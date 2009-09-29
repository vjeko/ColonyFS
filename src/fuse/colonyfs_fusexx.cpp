
#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"

#include <string>
#include <cstring>
#include <iostream>
#include <assert.h>
#include <string.h>

#include <boost/foreach.hpp>




colonyfs_fusexx::colonyfs_fusexx() {

  using namespace colony::xmlrpc;


  const std::string root_path("/");

  std::cout << "Initializing!" << std::endl;

  // Create the attribute associated with the root directory.
  //shared_ptr<attribute_value> pair = make_shared<attribute_value>(root_path);
  shared_ptr<attribute_value> pair = metadata_sink_[root_path];
  fattribute& attribute = pair->get_mapped();

  // What is the access mode?
  attribute.stbuf.st_mode = S_IFDIR | 0777;

  // What is the total number of entries within the directory?
  attribute.stbuf.st_nlink = 1;

  // Time of last modification?
  clock_gettime(CLOCK_REALTIME, &attribute.stbuf.st_mtim);

  // Commit.
  DHT::Instance().set_pair(pair);

  //metadata_sink_.flush();

}




metadata_sink_t colonyfs_fusexx::metadata_sink_;
colony::aggregator<colony::storage::chunk_data>  colonyfs_fusexx::data_sink_;
rlog::RLogChannel* colonyfs_fusexx::fuse_control_( RLOG_CHANNEL( "fuse/control" ) );
