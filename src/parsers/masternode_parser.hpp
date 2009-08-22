/*
 * masternode_parser.hpp
 *
 *  Created on: Jan 21, 2009
 *      Author: vjeko
 */

#ifndef MASTERNODE_PARSER_HPP_
#define MASTERNODE_PARSER_HPP_

#include "parser.hpp"

namespace colony { namespace parser {

class masternode_parser : public parser {
public:

	typedef std::vector< std::string > path_t;
	typedef std::vector< std::string > chunkservers_t;

	masternode_parser(boost::filesystem::path&);
	virtual ~masternode_parser();
	void check();

	const path_t          get_path();
	const chunkservers_t  get_chunkservers();

private:

	path_t          path_;
	chunkservers_t  chunkservers_;

};

} } // Namespace

#endif /* MASTERNODE_PARSER_HPP_ */
