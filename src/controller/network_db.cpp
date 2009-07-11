#include "network_db.hpp"

namespace uledfs { namespace db {

network_db::network_db() {}

void network_db::insert(uledfs::storage::network_metadata& c) { database.insert(c); }

void network_db::insert(
    uledfs::storage::network_metadata::uid_type& uid,
    uledfs::storage::network_metadata::cuid_type cuid,
    uledfs::storage::network_metadata::host_type& host) {
  database.insert( uledfs::storage::network_metadata(uid, cuid, host) );
}

void network_db::invalidate_host(uledfs::storage::network_metadata::host_type host) {

  host_view& host_set = database.get<host_tag>();
  host_set.erase(host);

}

network_db::range_iterator network_db::find_chunk(
    uledfs::storage::basic_metadata& basic_metadata) {

    return network_db::find_chunk(basic_metadata.uid_, basic_metadata.cuid_);
}

network_db::range_iterator network_db::find_chunk(
  uledfs::storage::network_metadata::uid_type& uid,
  uledfs::storage::network_metadata::cuid_type cuid) {

  basic_metadata_view& metadata_set = database.get<basic_metadata_tag>();
  return metadata_set.equal_range(boost::make_tuple(uid, cuid));
}

void network_db::print() {

  _db_impl::const_iterator it;
  for (it = database.begin(); it != database.end(); ++it) {
    it->print();
  }

} }

} // Namespace
