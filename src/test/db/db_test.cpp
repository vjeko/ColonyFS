/*
 * db_test.cpp
 *
 *  Created on: Aug 20, 2008
 *      Author: vjeko
 */

#include "../../controller/network_db.hpp"
#include "../../storage/network_metadata.hpp"

int main(int argc, char* argv[]) {

  using uledfs::storage::network_metadata;
  using uledfs::db::network_db;

  network_db _db;

  _db.insert( network_metadata("myfile1", 1, "192.168.10.1") );
  _db.insert( network_metadata("myfile1", 2, "192.168.10.2") );
  _db.insert( network_metadata("myfile1", 3, "192.168.10.3") );
  _db.insert( network_metadata("myfile1", 1, "192.168.10.4") );
  _db.insert( network_metadata("myfile1", 1, "192.168.10.1") );

  printf("Initial database: \n");
  _db.print();

  network_metadata::host_type host = "192.168.10.1";
  printf("\nRemoving host %s... \n", host.c_str());
  _db.invalidate_host("192.168.10.1");
  _db.print();

  network_metadata::uid_type   my_uid("myfile1");
  network_metadata::cuid_type  my_cuid(1);
  printf("\nWho has [%s][%d]?\n", my_uid.c_str(), my_cuid);
  network_db::range_iterator range = _db.find_chunk(my_uid, my_cuid);

  while (range.first != range.second){
      range.first->print();
      ++range.first;
      }

}
