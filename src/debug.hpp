/*
 * debug.hpp
 *
 *  Created on: Jun 27, 2008
 *      Author: vjeko
 */

#ifndef DEBUG_HPP_
#define DEBUG_HPP_

#include <boost/type_traits.hpp>
#include <boost/make_shared.hpp>

#include <rlog/rlog.h>
#include <rlog/RLogChannel.h>

#include <rlog/Error.h>
#include <rlog/StdioNode.h>
#include <rlog/RLogTime.h>



//TODO: Foreach does not belong here.
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

using boost::make_shared;

#endif /* DEBUG_HPP_ */
