/*
 * exceptions.hpp
 *
 *  Created on: Sep 23, 2009
 *      Author: vjeko
 */

#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <boost/exception.hpp>

namespace colony {

  class lookup_e : public boost::exception {};
  class cache_miss_e : public boost::exception {};

  class inexistent_file_e : public boost::exception {};

}

#endif /* EXCEPTIONS_HPP_ */
