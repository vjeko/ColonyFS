/*
 * chunkserver.cpp
 *
 *  Created on: Jun 27, 2008
 *      Author: vjeko
 */

#include "../../api/chunkserver_api.hpp"

/* SIGINT helper function. */
void _sig_h(int sig) {
  printf("caught signal %d... exiting\n", sig);
  exit(5);

  /*
   * SIG_DFL specifies the default action for the particular signal.
   *
   * signal(SIGINT, SIG_DFL);
   */
}

int main(int argc, char* argv[]) {

  /* Make sure to catch SIGINT (CTRL-C) interrupt. */
  signal(SIGINT, _sig_h);

  rlog::StdioNode chunkserver_log;

  // subscribe to various things individually
  chunkserver_log.subscribeTo( RLOG_CHANNEL("info") );
  chunkserver_log.subscribeTo( RLOG_CHANNEL("warning") );
  chunkserver_log.subscribeTo( RLOG_CHANNEL("error") );

  chunkserver_log.subscribeTo(RLOG_CHANNEL("intercom/server/control"));
  chunkserver_log.subscribeTo(RLOG_CHANNEL("intercom/chunkserver_harmony/control"));
  chunkserver_log.subscribeTo(RLOG_CHANNEL("parser/control"));

  boost::filesystem::path config_path("conf/chunkserver.conf");
  colony::chunkserver_api chunkserver(config_path);

  chunkserver.run();
}
