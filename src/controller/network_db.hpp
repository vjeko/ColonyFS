/*
 * network_db.hpp
 *
 *  Created on: Aug 30, 2008
 *      Author: vjeko
 */

#ifndef NETWORK_DB_HPP_
#define NETWORK_DB_HPP_

#include "../storage/basic_metadata.hpp"
#include "../storage/network_metadata.hpp"

#include <string>
#include <iostream>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace uledfs { namespace db {

namespace {

using boost::multi_index_container;
using namespace boost::multi_index;

/* Multi-Index Tags. */
struct basic_metadata_tag{};
struct host_tag{};

typedef boost::multi_index_container<
  uledfs::storage::network_metadata,
  indexed_by<
    ordered_non_unique<
      tag<basic_metadata_tag>,
      composite_key<
        uledfs::storage::network_metadata,
        BOOST_MULTI_INDEX_MEMBER(uledfs::storage::basic_metadata, std::string, uid_ ),
        BOOST_MULTI_INDEX_MEMBER(uledfs::storage::basic_metadata,         int, cuid_)
      >
    >,
    ordered_non_unique<
      tag<host_tag>,
      member<
        uledfs::storage::network_metadata,
        std::string, &uledfs::storage::network_metadata::host_
      >
    >
  >
> _db_impl;

}

class network_db {

/* Typedefs */
private:

  _db_impl database;

public:

  typedef std::pair<_db_impl::iterator, _db_impl::iterator> range_iterator;

  typedef _db_impl::index<basic_metadata_tag>::type        basic_metadata_view;
  typedef _db_impl::index<host_tag>::type                  host_view;

  /* Functions */

  network_db();

  void insert(uledfs::storage::network_metadata&);

  void insert(
      uledfs::storage::network_metadata::uid_type&,
      uledfs::storage::network_metadata::cuid_type,
      uledfs::storage::network_metadata::host_type&);

  void invalidate_host(uledfs::storage::network_metadata::host_type);

  range_iterator find_chunk(
    uledfs::storage::network_metadata::uid_type&,
    uledfs::storage::network_metadata::cuid_type);

  range_iterator find_chunk(uledfs::storage::basic_metadata&);

  void print();

};

} } // Namespace

#endif /* NETWORK_DB_HPP_ */
