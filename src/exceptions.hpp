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




  /** Generic Exceptions **/
  class lookup_e : public boost::exception {};




  /** Cache Exceptions **/
  class cache_miss_e : public boost::exception {};




  /** Local FS Exceptions **/
  class inexistent_file_e : public boost::exception {};
  class invalid_file_e : public boost::exception {};
  typedef boost::error_info<struct filepath, std::string> filepath_i;




}

#endif /* EXCEPTIONS_HPP_ */
