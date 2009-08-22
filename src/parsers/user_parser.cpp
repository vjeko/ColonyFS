/*
 * chunkserver_parser.cpp
 *
 *  Created on: Jan 18, 2009
 *      Author: vjeko
 */

#include "user_parser.hpp"

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>

namespace colony { namespace parser {

user_parser::user_parser(
		boost::filesystem::path& config_path) :
	parser(config_path) {

	config_.add_options()
		("user.masternode",
				boost::program_options::value< user_parser::masternode_t >(&masternode_)->composing(),
				"masternode's URI")
	    ;

	// Read data from file as input stream.
	boost::filesystem::ifstream ifs(config_path);

	// Store content of the configuration file into the variable map.
	boost::program_options::store(
			boost::program_options::parse_config_file(ifs, config_),
			vm_);

	boost::program_options::notify(vm_);

	/* Check for requirements. */
	parser::check();
	this->check();
}

user_parser::~user_parser() {}

void user_parser::check() {
	if (!vm_.count("user.masternode"))
		throw field_not_specified_error() << field_info("data.masternode");
}

const user_parser::masternode_t user_parser::get_masternode() {
	return masternode_;
}

} } // end namespace
