/*
 * client.cpp
 *
 *  Created on: Feb 21, 2009
 *      Author: vjeko
 */

#include "user_api.hpp"

#include "../storage/chunk_data.hpp"
#include "../storage/util.hpp"
#include "../xmlrpc/values.hpp"

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/function.hpp>

namespace uledfs {

user_api::user_api(
    parser_t& parser) :

      parser_(parser),
      dht_(parser_.get_dht_url()),
      intercom_(io_service_, parser_, dht_),
      user_log_( RLOG_CHANNEL( "user" ) ) {

  rLog(user_log_, "instantiating a public client");

}

user_api::~user_api() {}

void user_api::read(
    boost::filesystem::path& in,
    boost::filesystem::path& out) {

  using namespace uledfs::xmlrpc;

  rLog(user_log_, "retrieving filename <%s>...", in.leaf().c_str() );

  // Retrieve the filename information.
  filename_value  file      = dht_.get_value<filename_value>(in.leaf());
  std::string filename  = in.leaf();
  int number = boost::lexical_cast<size_t>( file.get_value() );

  rLog(user_log_, "... total number of chunks is %d\n", number );

  std::vector<boost::shared_ptr<uledfs::storage::chunk_data> > data_ptr_pool;

  // Retrieve each of the chunks and add them to the collection.
  for (size_t i = 0; i < number; i++) {
    boost::shared_ptr<storage::chunk_data> data_ptr(new storage::chunk_data(filename, i));
    data_ptr_pool.push_back(data_ptr);

    io_service_.post(
        boost::bind(&uledfs::intercom::user_harmony::retrieve_chunk, &intercom_, data_ptr)
    );

    usleep(10000);
    boost::thread(
        boost::bind(&boost::asio::io_service::run, &io_service_)
    );
  }

  io_service_.run();

  uledfs::storage::async_assemble_chunks(out, data_ptr_pool);

}

void user_api::write(boost::filesystem::path& in) {

  // Define the collection of chunks.
  boost::shared_ptr<std::vector<uledfs::storage::chunk_data> > data_pool_ptr(
      new std::vector<uledfs::storage::chunk_data>);

  // Chunk the designated file and populate the container.
  data_pool_ptr = uledfs::storage::chunk_file(in);

  // Deposit filename information.
  //xmlrpc::value_t value(boost::lexical_cast<xmlrpc::value_t>(data_pool_ptr->size()));

  xmlrpc::filename_value v(in.leaf(), data_pool_ptr->size());

  dht_.set_pair<xmlrpc::filename_value>(v);
  rLog(user_log_, "depositing file metadata <%s><%s>", in.leaf().c_str(), v.get_value().c_str() );

  foreach(uledfs::storage::chunk_data& chunk, *data_pool_ptr) {

    boost::shared_ptr<uledfs::storage::chunk_data> chunk_ptr(new uledfs::storage::chunk_data);
    *chunk_ptr = chunk;

    io_service_.post(
        boost::bind(&uledfs::intercom::user_harmony::deposit_chunk, &intercom_, chunk_ptr)
    );

    usleep(10000);
    boost::thread(
        boost::bind(&boost::asio::io_service::run, &io_service_)
    );

  }

  io_service_.run();
}

}
