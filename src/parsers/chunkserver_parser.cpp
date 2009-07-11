/*
 * chunkserver_parser.cpp
 *
 *  Created on: Jan 18, 2009
 *      Author: vjeko
 */

#include "chunkserver_parser.hpp"

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>

namespace uledfs { namespace parser {

chunkserver_parser::chunkserver_parser(
		boost::filesystem::path& config_path) :
	parser(config_path) {

	config_.add_options()
    ("data.path",
        boost::program_options::value< chunkserver_parser::path_t >(&data_path_)->composing(),
        "path(s) that will be used for storing data")
		("data.masternode",
				boost::program_options::value< std::string >(&masternode_)->composing(),
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

chunkserver_parser::~chunkserver_parser() {}

void chunkserver_parser::check() {
	if (!vm_.count("data.path"))
		throw field_not_specified_error() << field_info("data.path");
	if (!vm_.count("data.masternode"))
		throw field_not_specified_error() << field_info("data.masternode");
}

void chunkserver_parser::print() {
	if (vm_.count("data.path")) {
		BOOST_FOREACH(std::string& var, data_path_) {
			std::cout << var << std::endl;
		}
	}
}

const chunkserver_parser::path_t chunkserver_parser::get_path() {
	return data_path_;
}

const chunkserver_parser::masternode_t chunkserver_parser::get_masternode() {
	return masternode_;
}

} } // end namespace
