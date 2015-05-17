//
// client.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2009 Xu Ye Jun (moore.xu@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BAS_CLIENT_HPP
#define BAS_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include "io_service_pool.hpp"
#include "service_handler.hpp"
#include "service_handler_pool.hpp"

namespace bas {

/// The top-level class of the client.
template<typename Work_Handler, typename Work_Allocator, typename Socket_Service = boost::asio::ip::tcp::socket>
class client
  : private boost::noncopyable
{
public:
  /// Define type reference of boost::asio::io_service.
  typedef boost::asio::io_service io_service_type;

  /// Define type reference of boost::asio::ip::tcp::endpoint.
  typedef boost::asio::ip::tcp::endpoint endpoint_type;

  /// The type of the service_handler.
  typedef service_handler<Work_Handler, Socket_Service> service_handler_type;
  typedef boost::shared_ptr<service_handler_type> service_handler_ptr;

  /// The type of the service_handler_pool.
  typedef service_handler_pool<Work_Handler, Work_Allocator, Socket_Service> service_handler_pool_type;
  typedef boost::shared_ptr<service_handler_pool_type> service_handler_pool_ptr;

  /// Construct the client object for connect to specified TCP address and port.
  explicit client(service_handler_pool_type* service_handler_pool,
      const std::string& address,
      unsigned short port)
    : service_handler_pool_(service_handler_pool),
      endpoint_(boost::asio::ip::address::from_string(address), port)
  {
    BOOST_ASSERT(service_handler_pool != 0);

    // Create preallocated handlers of the pool.
    service_handler_pool_->init();
  }

  /// Construct the client object for connect to specified TCP address and port.
  client(service_handler_pool_type* service_handler_pool)
    : service_handler_pool_(service_handler_pool)
  {
    BOOST_ASSERT(service_handler_pool != 0);

    // Create preallocated handlers of the pool.
    service_handler_pool_->init();
  }

  /// Destruct the client object.
   ~client()
  {
    // Release all handlers in the pool.
    service_handler_pool_->close();

    // Destroy service_handler pool.
    service_handler_pool_.reset();
  }

  /// Make an connection with given io_service and work_service.
  bool connect(io_service_type& io_service,
      io_service_type& work_service,
      endpoint_type& endpoint)
  {
    // Get new handler for connect.
    service_handler_ptr new_handler = service_handler_pool_->get_service_handler(io_service,
        work_service);

    if (new_handler.get() == 0)
      return false;

    // Use new handler to connect.
    new_handler->connect(endpoint);

    return true;
  }

  /// Make an connection with the given parent_handler.
  template<typename Parent_Handler>
  bool connect(Parent_Handler& parent_handler,
      endpoint_type& endpoint)
  {
    // Get new handler for connect.
    service_handler_ptr new_handler = service_handler_pool_->get_service_handler(parent_handler.io_service(),
        parent_handler.work_service());

    if (new_handler.get() == 0)
      return false;

    // Execute in work_thread, because connect will be called in the same thread.
    parent_handler.set_child(new_handler.get());
    new_handler->set_parent(&parent_handler);
    
    // Use new handler to connect.
    new_handler->connect(endpoint);

    return true;
  }

  /// Make an connection with given io_service and work_service.
  bool connect(io_service_type& io_service,
      io_service_type& work_service)
  {
    // Connect with the internal endpoint.
    return connect(io_service, work_service, endpoint_);
  }

  /// Make an connection to specific host with given io_service and work_service.
  bool connect(io_service_type& io_service,
      io_service_type& work_service,
      const std::string& address,
      unsigned short port)
  {
    // Prepare endpoint for connect.
    endpoint_type endpoint(boost::asio::ip::address::from_string(address), port);

    // Connect with the given endpoint.
    return connect(io_service, work_service, endpoint);
  }

  /// Make an connection with the given parent_handler.
  template<typename Parent_Handler>
  bool connect(Parent_Handler& parent_handler)
  {
    // Connect with the internal endpoint.
    return connect(parent_handler, endpoint_);
  }

  /// Make an connection to specific host with the given parent_handler.
  template<typename Parent_Handler>
  bool connect(Parent_Handler& parent_handler,
      const std::string& address,
      unsigned short port)
  {
    // Prepare endpoint for connect.
    endpoint_type endpoint(boost::asio::ip::address::from_string(address), port);

    // Connect with the given endpoint.
    return connect(parent_handler, endpoint);
  }

private:
  /// The pool of service_handler objects.
  service_handler_pool_ptr service_handler_pool_;

  /// The server endpoint.
  endpoint_type endpoint_;
};

} // namespace bas

#endif // BAS_CLIENT_HPP
