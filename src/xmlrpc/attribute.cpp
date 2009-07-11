/*
 * attribute.cpp
 *
 *  Created on: May 15, 2009
 *      Author: vjeko
 */

#include "attribute.hpp"

fattribute::fattribute() {
  memset(&stbuf, 0, sizeof(struct stat));
}

fattribute::~fattribute() {

}
