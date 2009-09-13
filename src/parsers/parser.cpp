/*
 * parser.cpp
 *
 *  Created on: Jan 18, 2009
 *      Author: vjeko
 */

#include "parser.hpp"

#include <fstream>

#include <boost/filesystem/fstream.hpp>

namespace colony { namespace parser {

parser::parser(boost::filesystem::path config_path) :
	config_("Configuration"),
	parser_control_(RLOG_CHANNEL( "parser/control" )){

  rLog(parser_control_,"parsing configuration file...");

	/*
	 * Make sure that the file exists, otherwise,
	 * we have to throw an exception.
	 */
	if (!boost::filesystem::exists(config_path)) {
	  rError("... configuration file not found");
		throw file_open_error() << filename_info(config_path.string());
	} else {
	  rLog(parser_control_, "... %s", config_path.string().c_str());
	}


	/* Add general options, common to every type of parser. */
	config_.add_options()
			/* Name of the machine */
			("general.name",
					boost::program_options::value< name_t >(&name_),
					"name of the node")
			/* Maximum disk quota used by specified node. */
			("general.quota",
					boost::program_options::value< quota_t >(&quota_),
					"maximum disk quota that can be used")
			/* Name of the swarm. */
      ("general.swarm",
          boost::program_options::value< std::string >(&swarm_),
          "swarm name")
      /* DHT entry node. */
      ("general.dht",
          boost::program_options::value< std::string >(&dht_url_),
          "dht entry node")
	;

}

void parser::check() {
	if (!vm_.count("general.name"))
		throw field_not_specified_error() << field_info("general.name");

	if (!vm_.count("general.quota"))
		throw field_not_specified_error() << field_info("general.quota");
}

parser::~parser() {}

const boost::program_options::options_description parser::get_program_options() { return config_; }
const parser::name_t parser::get_name() { return name_; }
const parser::quota_t parser::get_quota() {return quota_; }
const std::string  parser::get_dht_url() { return dht_url_; }
const std::string  parser::get_swarm() { return swarm_; }

} }
