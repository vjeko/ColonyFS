/*
 * intercom.hpp
 *
 *  Created on: Jun 28, 2008
 *      Author: vjeko
 */

#ifndef INTERCOM_HPP_
#define INTERCOM_HPP_

#include "../storage/chunk_metadata.hpp"
#include "../storage/chunk_data.hpp"

#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/function.hpp>

#include <boost/fusion/container.hpp>
#include <boost/fusion/include/container.hpp>

#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>

#include <boost/fusion/container/generation/make_map.hpp>
#include <boost/fusion/include/make_map.hpp>

enum command_t {
	DEPOSIT_CHUNK = 2,
	RETRIEVE_CHUNK = 4,
	HEARTBEAT = 16,

	LOCK_CHUNK = 32,
	READ_REQUEST = 64
};

enum port_t {
  MASTERNODE_PORT      = 1213,
  CHUNK_SERVER_PORT    = 1212
};

//------------------------------------------------------------------------------

typedef boost::variant<std::string, int>  content_t;
typedef boost::shared_ptr<content_t>      content_ptr_t;

typedef boost::fusion::pair<
  boost::mpl::int_<DEPOSIT_CHUNK>,
  boost::function<void (content_ptr_t)>
> int_1_callback_fusion_t;

typedef boost::fusion::pair<
  boost::mpl::int_<RETRIEVE_CHUNK>,
  boost::function<void (content_ptr_t)>
> int_2_callback_fusion_t;

typedef boost::fusion::result_of::make_map<
  int_1_callback_fusion_t::first_type,
  int_2_callback_fusion_t::first_type,

  int_1_callback_fusion_t::second_type,
  int_2_callback_fusion_t::second_type
>::type callback_map_t;

//--------------------------------------

typedef boost::fusion::pair<
  boost::mpl::int_<DEPOSIT_CHUNK>,
  boost::function<void (colony::storage::chunk_data)>
> masternode_deposit_chunk_callback_t;

typedef boost::fusion::pair<
  boost::mpl::int_<RETRIEVE_CHUNK>,
  boost::function<void (colony::storage::chunk_metadata)>
> masternode_retrieve_chunk_callback_t;

typedef boost::fusion::result_of::make_map<
  masternode_deposit_chunk_callback_t::first_type,
  masternode_retrieve_chunk_callback_t::first_type,

  masternode_deposit_chunk_callback_t::second_type,
  masternode_retrieve_chunk_callback_t::second_type
>::type masternode_callback_map_t;

#endif /* INTERCOM_HPP_ */
