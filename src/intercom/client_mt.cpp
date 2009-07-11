#include "client_mt.hpp"

namespace uledfs {

namespace intercom {

client_mt::client_mt(boost::asio::io_service& io_service) :
    client_mt_control_( RLOG_CHANNEL( "intercom/client_mt/control" ) ),
    io_service_(io_service) {

  rLog(client_mt_control_, "initializing");

}

client_mt::~client_mt() {};

std::string client_mt::service_ = "1212";

}

}
