/*
 * CSScheduler.hpp
 *
 *  Created on: Sep 22, 2009
 *      Author: vjeko
 */

#ifndef CSSCHEDULER_HPP_
#define CSSCHEDULER_HPP_

#include <string>



namespace colony {


/*
 * FIXME: Avoid singleton pattern!
 * TODO: Load balancing.
 */
class CSScheduler {

public:

  static const std::string GetCS() {
    const std::string hostname("borg");
    return hostname;
  }




private:

  CSScheduler() {};

};

}

#endif /* CSSCHEDULER_HPP_ */
