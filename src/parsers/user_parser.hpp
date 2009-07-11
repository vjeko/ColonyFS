/*
 * chunkserver_parser.hpp
 *
 *  Created on: Jan 18, 2009
 *      Author: vjeko
 */

#ifndef USER_PARSER_HPP_
#define USER_PARSER_HPP_

#include "parser.hpp"

#include <string>
#include <vector>

#include <boost/exception.hpp>
#include <boost/filesystem.hpp>

namespace uledfs { namespace parser {

class user_parser : public parser {
public:
	typedef std::vector< std::string > path_t;
	typedef std::string masternode_t;

	user_parser(boost::filesystem::path&);
	virtual ~user_parser();

	void check();
	void print();

	/* Getters. */
	const masternode_t get_masternode();

private:
	path_t        data_path_;
	masternode_t  masternode_;

};

} }

#endif /* CHUNKSERVER_PARSER_H_ */
