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

#include <tbb/concurrent_hash_map.h>
#include <tbb/mutex.h>
#include <tbb/spin_mutex.h>
#include <tbb/spin_rw_mutex.h>
#include <tbb/queuing_rw_mutex.h>

#include <map>


struct CV {

  typedef tbb::spin_mutex                  mutex_type;

  template<typename T>
  static mutex_type& Instance(boost::shared_ptr<T> pair) {

    typedef typename T::key_type             key_type;
    typedef std::map<key_type, mutex_type>   mutex_map_type;

    static mutex_map_type g_mutex_map;
    return g_mutex_map[pair->get_key()];

  }

};



#endif /* SYNCHRONIZATION_HPP_ */
