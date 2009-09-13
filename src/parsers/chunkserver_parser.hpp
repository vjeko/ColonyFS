/*
 * chunkserver_parser.hpp
 *
 *  Created on: Jan 18, 2009
 *      Author: vjeko
 */

#ifndef CHUNKSERVER_PARSER_HPP_
#define CHUNKSERVER_PARSER_HPP_

#include "parser.hpp"

#include <string>
#include <vector>

#include <boost/exception.hpp>
#include <boost/filesystem.hpp>

namespace colony { namespace parser {

class chunkserver_parser : public parser {
public:
	typedef std::vector< std::string > path_t;
	typedef std::string masternode_t;

	chunkserver_parser(boost::filesystem::path);
	virtual ~chunkserver_parser();

	void check();
	void print();

	/* Getters. */
	const path_t       get_path();
	const masternode_t get_masternode();

private:
	path_t        data_path_;
	std::string   masternode_;
};

} }

#endif /* CHUNKSERVER_PARSER_H_ */
