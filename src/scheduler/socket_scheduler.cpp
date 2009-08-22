/*
 * socket_scheduler.cpp
 *
 *  Created on: Mar 10, 2009
 *      Author: vjeko
 */

#include "socket_scheduler.hpp"

#include <boost/thread.hpp>

namespace colony {
namespace scheduler {

socket_scheduler::socket_scheduler(size_t max_concurency) :
      pool_(max_concurency) {

}

socket_scheduler::~socket_scheduler() {}

}
} // Namespace.
