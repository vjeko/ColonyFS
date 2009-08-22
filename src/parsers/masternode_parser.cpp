/*
 * masternode_parser.cpp
 *
 *  Created on: Jan 21, 2009
 *      Author: vjeko
 */

#include "masternode_parser.hpp"

#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>

namespace colony { namespace parser {

masternode_parser::masternode_parser(
		boost::filesystem::path& config_path) : parser(config_path) {

		  config_.add_options()
          ("metadata.path",
            boost::program_options::value< masternode_parser::path_t >(&path_)->composing(),
            "path(s) that will be used for storing metadata")
		      ("metadata.chunkserver",
						boost::program_options::value< masternode_parser::chunkservers_t >(&chunkservers_)->composing(),
						"list of chunkservers")
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

masternode_parser::~masternode_parser() {}

void masternode_parser::check() {
	if (!vm_.count("metadata.path"))
		throw field_not_specified_error() << field_info("data.path");
	if (!vm_.count("metadata.chunkserver"))
		throw field_not_specified_error() << field_info("data.masternode");
}

const masternode_parser::path_t masternode_parser::get_path() {
  return path_;
}

const masternode_parser::chunkservers_t masternode_parser::get_chunkservers() {
  return chunkservers_;
}

} } // Namespace
