/*
 * parser_test.cpp
 *
 *  Created on: Sep 2, 2008
 *      Author: vjeko
 */

#include "../../parsers/chunkserver_parser.hpp"
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <iostream>

int main() {

	boost::filesystem::path cfg("conf/masternode.conf");
	uledfs::parser::chunkserver_parser parser(cfg);

}
