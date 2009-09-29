/*
 * fuse_rename.cpp
 *
 *  Created on: Sep 29, 2009
 *      Author: vjeko
 */


#include "../debug.hpp"
#include "colonyfs_fusexx.hpp"



int colonyfs_fusexx::rename(const char* oldpath, const char* newpath) {

  // TODO: Input validation checking. See glibc manual.

  using namespace colony::xmlrpc;


  rLog(fuse_control_, "rename: %s -> %s", oldpath, newpath);

  const boost::filesystem::path oldfull(oldpath);
  const boost::filesystem::path oldbranch = oldfull.parent_path();
  const boost::filesystem::path oldleaf = oldfull.filename();

  const boost::filesystem::path newfull(newpath);
  const boost::filesystem::path newbranch = newfull.parent_path();
  const boost::filesystem::path newleaf = newfull.filename();


  // Retrieve the attribute for the old branch path.
  shared_ptr<attribute_value> oldbranch_pair = metadata_sink_[ oldbranch.string() ];
  fattribute& oldbranch_attribute = oldbranch_pair->get_mapped();


  // Find the old leaf.
  metadata_sink_t::mapped_type::list_t::iterator oldbranch_iterator = std::find(
      oldbranch_attribute.list.begin(),
      oldbranch_attribute.list.end(),
      oldleaf.string()
      );

  // And erase it.
  oldbranch_attribute.list.erase(oldbranch_iterator);


  // Retrieve the attribute for the new branch path.
  shared_ptr<attribute_value> newbranch_pair = metadata_sink_[ newbranch.string() ];
  fattribute& newbranch_attribute = newbranch_pair->get_mapped();

  newbranch_attribute.list.push_back(newleaf.string());


  // Retrieve the attribute for the new path.
  shared_ptr<attribute_value> oldfull_pair =
      metadata_sink_( oldfull.string() );

  // Associate the old attribute with the new path.
  shared_ptr<attribute_value> newfull_pair = metadata_sink_[newfull.string()];
  newfull_pair->get_mapped() = oldfull_pair->get_mapped();


  metadata_sink_.erase( oldfull.string() );

  // FIXME: Implement data rename.
  data_sink_.rename(oldfull, newfull, newfull_pair->get_mapped().stbuf.st_size);

  return 0;

}
