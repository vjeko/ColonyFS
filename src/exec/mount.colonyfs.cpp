
#include "../fuse/colonyfs_fusexx.hpp"
#include "../debug.hpp"
#include "../algo/CSScheduler.hpp"

#include <iostream>
#include <sstream>

int main(int argc, char **argv) {


  rlog::StdioNode client_log(1, rlog::StdioNode::OutputColor);

  client_log.subscribeTo( RLOG_CHANNEL("debug") ) ;
  client_log.subscribeTo( RLOG_CHANNEL("error") ) ;
  client_log.subscribeTo( RLOG_CHANNEL("info") );
  client_log.subscribeTo( RLOG_CHANNEL("warning") );

  client_log.subscribeTo( RLOG_CHANNEL("fuse/control") ) ;
  client_log.subscribeTo( RLOG_CHANNEL("intercom/user_harmony/control") ) ;
  //client_log.subscribeTo( RLOG_CHANNEL("sink") ) ;

  colony::CSScheduler::SetCSL();

  colonyfs_fusexx hello;

  // The first 3 parameters are identical to the fuse_main function.
  // The last parameter gives a pointer to a class instance, which is
  // required for static methods to access instance variables/ methods.
  return hello.main(argc, argv, NULL, &hello);
}
