cmake_minimum_required(VERSION 2.6)

project (COLONY)

# RLog Component.
ADD_DEFINITIONS(-DRLOG_COMPONENT=”rlog”)

FILE(GLOB XMLRPC_CPP "xmlrpc/*.cpp")

add_library (xmlrpc SHARED ${XMLRPC_CPP}) 
target_link_libraries (xmlrpc 
  rlog curl 
  xmlrpc xmlrpc++ 
  xmlrpc_cpp 
  xmlrpc_client 
  xmlrpc_client++ 
  xmlrpc_server 
  xmlrpc_server++ 
  xmlrpc_xmlparse 
  xmlrpc_xmltok 
  xmlrpc_util)
