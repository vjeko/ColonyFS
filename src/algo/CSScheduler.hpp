/*
 * CSScheduler.hpp
 *
 *  Created on: Sep 22, 2009
 *      Author: vjeko
 */

#ifndef CSSCHEDULER_HPP_
#define CSSCHEDULER_HPP_

#include <string>
#include <boost/random.hpp>



namespace colony {


/*
 * FIXME: Avoid singleton pattern!
 * TODO: Load balancing.
 */
class CSScheduler {

private:

  CSScheduler() {};



public:

  static const std::string GetCS() {

    static std::vector<std::string> chunkservers_;

    chunkservers_.push_back("carol");
    chunkservers_.push_back("bob");
    chunkservers_.push_back("eve");
    chunkservers_.push_back("mallory");

    boost::mt19937 rng;
    boost::uniform_int<> size(0, chunkservers_.size() - 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > var_rng(rng, size);

    return chunkservers_[var_rng()];
  }

};

}

#endif /* CSSCHEDULER_HPP_ */
