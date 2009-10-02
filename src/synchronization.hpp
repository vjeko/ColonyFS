/*
 * synchronization.hpp
 *
 *  Created on: 2009-09-30
 *      Author: vjeko
 */

#ifndef SYNCHRONIZATION_HPP_
#define SYNCHRONIZATION_HPP_

#include "storage/chunk_data.hpp"


#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple_io.hpp>

#include <tbb/mutex.h>
#include <tbb/spin_rw_mutex.h>
#include <tbb/queuing_rw_mutex.h>

#include <map>


// FIXME: DO NOT USE GLOBAL VARIABLES!

struct Sync {

  typedef tbb::spin_rw_mutex mutex_type;
  typedef std::map<colony::storage::chunk_data::key_type, mutex_type> mutex_map_type;
  typedef std::map<std::string, mutex_type> flush_map;

  typedef colony::storage::chunk_data::key_type key_type;


  static flush_map& FM() {

    static flush_map mm;
    return mm;

  }


  static mutex_map_type& MM() {
    static mutex_map_type mm;
    return mm;
  }


	static void Lock(colony::storage::chunk_data::key_type key) {

	  MM()[key].lock();

	}

	static void Unlock(colony::storage::chunk_data::key_type key) {

		MM()[key].unlock();

	}

};



#endif /* SYNCHRONIZATION_HPP_ */
