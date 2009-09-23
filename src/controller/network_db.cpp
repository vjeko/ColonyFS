#include "network_db.hpp"

namespace colony { namespace db {

network_db::network_db() {}

void network_db::insert(colony::storage::network_metadata& c) { database.insert(c); }

void network_db::insert(
    colony::storage::network_metadata::uid_type& uid,
    colony::storage::network_metadata::cuid_type cuid,
    colony::storage::network_metadata::host_type& host) {
  database.insert( colony::storage::network_metadata(uid, cuid, host) );
}

void network_db::invalidate_host(colony::storage::network_metadata::host_type host) {

  host_view& host_set = database.get<host_tag>();
  host_set.erase(host);

}

network_db::range_iterator network_db::find_chunk(
    colony::storage::basic_metadata& basic_metadata) {

    return network_db::find_chunk(basic_metadata.uid_, basic_metadata.cuid_);
}

network_db::range_iterator network_db::find_chunk(
  colony::storage::network_metadata::uid_type& uid,
  colony::storage::network_metadata::cuid_type cuid) {

  basic_metadata_view& metadata_set = database.get<basic_metadata_tag>();
  return metadata_set.equal_range(boost::make_tuple(uid, cuid));
}


} } // Namespace
