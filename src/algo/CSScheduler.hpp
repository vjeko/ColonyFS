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

  typedef std::vector<std::string> csl_type;

  static csl_type& GetCSL() {
    static csl_type csl;
    return csl;
  }

public:

  static void SetCSL() {
    GetCSL().push_back("carol");
    GetCSL().push_back("bob");
    GetCSL().push_back("eve");
    GetCSL().push_back("mallory");
  }

  static int GetRandom(int start, int end) {

    static boost::mt19937 rng;
    static boost::uniform_int<> size(start, end);
    static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > var_rng(rng, size);

    return var_rng();

  }

  static std::string GetCS() {

    static boost::mt19937 rng;
    static boost::uniform_int<> size(0, GetCSL().size() - 1);
    static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > var_rng(rng, size);

    return GetCSL()[var_rng()];
  }

};

}

#endif /* CSSCHEDULER_HPP_ */
