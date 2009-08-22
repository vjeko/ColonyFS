/*
 * intercom_test.cpp
 *
 *  Created on: Jun 27, 2008
 *      Author: vjeko
 */

//#include "../../intercom/masternode.hpp"
#include "../../debug.hpp"
#include "../../intercom/user_harmony.hpp"
#include "../../storage/util.hpp"
#include "../../parsers/user_parser.hpp"

#include "../../api/user_api.hpp"

#include <iostream>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

int main(int argc, char* argv[]) {

	rlog::StdioNode client_log;

	client_log.subscribeTo( RLOG_CHANNEL("debug") ) ;
  client_log.subscribeTo( RLOG_CHANNEL("error") ) ;
  client_log.subscribeTo( RLOG_CHANNEL("info") );
  client_log.subscribeTo( RLOG_CHANNEL("warning") );

  client_log.subscribeTo( RLOG_CHANNEL("user") ) ;
  client_log.subscribeTo( RLOG_CHANNEL("intercom/user_harmony/control") ) ;
	client_log.subscribeTo( RLOG_CHANNEL("intercom/client_mt/control") ) ;

  boost::filesystem::path config_path("conf/user.conf");
  colony::parser::user_parser parser(config_path);
  colony::user_api client(parser);

  /*
   * Program options.
   */
  std::string read = "READ";
  std::string write = "WRITE";

  boost::program_options::options_description cli("Command line options");
  cli.add(parser.get_program_options());
  cli.add_options()
    ("help,h", "print usage message")
    ("write,w", boost::program_options::value<std::string>(&write), "output file")
    ("read,r", boost::program_options::value<std::string>(&read), "input file")
    ;

  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, cli),
      vm
      );
  boost::program_options::notify(vm);

  if (vm.count("help")) {
      std::cout << cli << std::endl;
      exit(0);
  }

  if ((!vm.count("write") && !vm.count("read"))) {
      rError("no read/write operation specified... exiting");
      exit(0);
  }

  /* End Program Options. */

  if (vm.count("write")) {

    boost::filesystem::path write_file(write);
    if (!boost::filesystem::exists(write_file)) {
      rDebug("unable to locate <%s>... exiting", write_file.string().c_str());
      exit(6);
    } else {
      rDebug("writing <%s> to DFS", write_file.string().c_str());
    }

    boost::filesystem::path in(write_file);
    client.write(in);
  }


  if (vm.count("read")) {

    boost::filesystem::path write_file(read);

    boost::filesystem::path in(write_file);
    boost::filesystem::path out(write_file);

    client.read(in, out);
  }

}
