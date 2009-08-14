/*
 * attribute.hpp
 *
 *  Created on: May 15, 2009
 *      Author: vjeko
 */

#ifndef ATTRIBUTE_HPP_
#define ATTRIBUTE_HPP_

#include <cstring>
#include <sys/stat.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>

class fattribute {

public:

  typedef std::list<std::string> list_t;

  list_t      list;
  struct stat stbuf;

  fattribute();

  ~fattribute();

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {

    ar & stbuf.st_dev;
    ar & stbuf.st_ino;

    ar & stbuf.st_mode;
    ar & stbuf.st_size;

    ar & stbuf.st_mtim.tv_sec;
    ar & stbuf.st_mtim.tv_nsec;

    ar & stbuf.st_uid;
    ar & stbuf.st_gid;

    ar & list;
  }

};

#endif /* ATTRIBUTE_HPP_ */
