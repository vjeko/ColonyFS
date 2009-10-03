/*
 * parser.hpp
 *
 *  Created on: Jan 18, 2009
 *      Author: vjeko
 */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "../debug.hpp"

#include <boost/program_options.hpp>
#include <boost/cstdint.hpp>
#include <boost/exception.hpp>
#include <boost/filesystem.hpp>

namespace colony { namespace parser {

typedef boost::error_info<struct tag_filename, std::string> filename_info;
class file_open_error: public boost::exception { };

typedef boost::error_info<struct tag_field, std::string> field_info;
class field_not_specified_error: public boost::exception { };


/*
 * This is the base class for any type of parser. Its purpose
 * is to minimize code duplication by moving common variables
 * to this class. Those variables are located under 'general'
 * section.
 */
class parser {
protected:
	typedef std::string    name_t;
	typedef boost::int32_t quota_t;

public:
	parser(boost::filesystem::path);
	virtual ~parser();

	/*
	 * This function makes sure that all required fields are set.
	 */
	virtual void check() = 0;

	virtual boost::program_options::options_description get_program_options();

	/* Getters. */
	virtual name_t get_name();
	virtual quota_t get_quota();
	virtual std::string get_dht_url();
	virtual std::string get_swarm();

protected:

	boost::program_options::options_description config_;

	// The variables_map class is used to store the option values.
	boost::program_options::variables_map vm_;

	name_t      name_;
	quota_t     quota_;
	std::string swarm_;
	std::string dht_url_;

	rlog::RLogChannel *parser_control_;


};

} } //namespace

#endif /* PARSER_HPP_ */
