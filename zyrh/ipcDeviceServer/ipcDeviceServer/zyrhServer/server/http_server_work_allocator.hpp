//
// server_work_allocator.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2009 Xu Ye Jun (moore.xu@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BAS_ECHO_HTTP_SERVER_WORK_ALLOCATOR_HPP
#define BAS_ECHO_HTTP_SERVER_WORK_ALLOCATOR_HPP
#include "server_work_allocator.hpp"
#include "http_server_work.h"



class http_server_work_allocator:server_work_allocator
{
public:


  http_server_work_allocator()
  {
  }
  server_work* make_handler()
  {
    return (server_work*)new http_server_work();
  }
};


#endif // BAS_ECHO_HTTP_SERVER_WORK_ALLOCATOR_HPP
