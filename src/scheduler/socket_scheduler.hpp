/*
 * socket_scheduler.hpp
 *
 *  Created on: Mar 10, 2009
 *      Author: vjeko
 */

#ifndef SOCKET_SCHEDULER_HPP_
#define SOCKET_SCHEDULER_HPP_

#include <string>

#include "../boost/threadpool.hpp"

namespace colony { namespace scheduler {

//template<typename F>
class socket_scheduler {
public:
	socket_scheduler(size_t max_concurency);
	virtual ~socket_scheduler();

private:
  boost::threadpool::fifo_pool pool_;
};

} } // Namespace.

#endif /* SOCKET_SCHEDULER_HPP_ */
