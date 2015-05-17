//
// service_handler_pool.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Xu Ye Jun (moore.xu@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BAS_SERVICE_HANDLER_POOL_HPP
#define BAS_SERVICE_HANDLER_POOL_HPP

#include <boost/assert.hpp>
#include <boost/asio/detail/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

#include "service_handler.hpp"

namespace bas {

#define BAS_HANDLER_POOL_INIT_SIZE       100
#define BAS_HANDLER_POOL_LOW_WATERMARK   0
#define BAS_HANDLER_POOL_HIGH_WATERMARK  5000
#define BAS_HANDLER_POOL_INCREMENT       10
#define BAS_HANDLER_POOL_MAXIMUM         50000

#define BAS_HANDLER_BUFFER_DEFAULT_SIZE  256
#define BAS_HANDLER_DEFAULT_TIMEOUT      30

/// A pool of service_handler objects.
template<typename Work_Handler, typename Work_Allocator, typename Socket_Service = boost::asio::ip::tcp::socket>
class service_handler_pool
  : public boost::enable_shared_from_this<service_handler_pool<Work_Handler, Work_Allocator, Socket_Service> >,
    private boost::noncopyable
{
public:
  using boost::enable_shared_from_this<service_handler_pool<Work_Handler, Work_Allocator, Socket_Service> >::shared_from_this;

  /// Define type reference of std::size_t.
  typedef std::size_t size_type;

  /// The type of the service_handler.
  typedef service_handler<Work_Handler, Socket_Service> service_handler_type;
  typedef boost::shared_ptr<service_handler_type> service_handler_ptr;

  /// The type of the work_allocator.
  typedef Work_Allocator work_allocator_type;
  typedef boost::shared_ptr<work_allocator_type> work_allocator_ptr;

  /// Construct the service_handler pool.
  explicit service_handler_pool(work_allocator_type* work_allocator,
      size_type pool_init_size = BAS_HANDLER_POOL_INIT_SIZE,
      size_type read_buffer_size = BAS_HANDLER_BUFFER_DEFAULT_SIZE,
      size_type write_buffer_size = 0,
      unsigned int session_timeout = BAS_HANDLER_DEFAULT_TIMEOUT,
      unsigned int io_timeout = 0,
      size_type pool_low_watermark = BAS_HANDLER_POOL_LOW_WATERMARK,
      size_type pool_high_watermark = BAS_HANDLER_POOL_HIGH_WATERMARK,
      size_type pool_increment = BAS_HANDLER_POOL_INCREMENT,
      size_type pool_maximum = BAS_HANDLER_POOL_MAXIMUM)
    : mutex_(),
      service_handlers_(),
      work_allocator_(work_allocator),
      read_buffer_size_(read_buffer_size),
      write_buffer_size_(write_buffer_size),
      session_timeout_(session_timeout),
      io_timeout_(io_timeout),
      pool_init_size_(pool_init_size),
      pool_low_watermark_(pool_low_watermark),
      pool_high_watermark_(pool_high_watermark),
      pool_increment_(pool_increment),
      pool_maximum_(pool_maximum),
      handler_count_(0),
      closed_(false)
  {
    BOOST_ASSERT(work_allocator != 0);
    BOOST_ASSERT(pool_init_size != 0);
    BOOST_ASSERT(pool_low_watermark <= pool_init_size);
    BOOST_ASSERT(pool_high_watermark > pool_low_watermark);
    BOOST_ASSERT(pool_maximum > pool_high_watermark);
    BOOST_ASSERT(pool_increment != 0);
 }

  /// Destruct the pool object.
  ~service_handler_pool()
  {
    work_allocator_.reset();
  }

  /// Create preallocated handler to the pool.
  ///   Note: shared_from_this() can't be used in the constructor.
  void init(void)
  {
    // Create preallocated handler to the pool.
    create_handler(pool_init_size_);

    closed_ = false;
  }

  /// Release all handlers in the pool.
  ///   Note: close() can't be used in the destructor.
  void close(void)
  {
    // Release all handlers in the pool.
    clear();
  }

  /// Get an service_handler to use.
  service_handler_ptr get_service_handler(boost::asio::io_service& io_service,
      boost::asio::io_service& work_service)
  {
    // Get a handler.
    service_handler_ptr service_handler = get_handler();

    // service_handler.get() = 0 when handlers count exceeded maximum.
    if (service_handler.get() != 0)
    {
      // Bind the handler with given io_service and work_service.
      service_handler->bind(io_service, work_service, work_allocator());
    }

    return service_handler;
  }

  /// Put a handler to the pool.
  void put_handler(service_handler_type* handler_ptr)
  {
    // Need lock in multiple thread model.
    boost::asio::detail::mutex::scoped_lock lock(mutex_);

    // If the pool has exceed high water mark or been closed, delete this handler.
    if (closed_ || (service_handlers_.size() >= pool_high_watermark_))
    {
      delete handler_ptr;
      --handler_count_;

      return;
    }

    // Push this handler into the pool.
    push_handler(handler_ptr);
  }

  /// Get the number of active handlers.
  size_type get_load(void)
  {
    // Need lock in multiple thread model.
    boost::asio::detail::mutex::scoped_lock lock(mutex_);

    return (handler_count_ - service_handlers_.size());
  }

  /// Get the count of the handlers.
  size_type handler_count(void)
  {
    // Need lock in multiple thread model.
    boost::asio::detail::mutex::scoped_lock lock(mutex_);

    return handler_count_;
  }

private:
  
  /// Get the allocator for work.
  work_allocator_type& work_allocator(void)
  {
    BOOST_ASSERT(work_allocator_.get() != 0);

    return *work_allocator_;
  }

  /// Release handlers in the pool.
  void clear(void)
  {
    // Need lock in multiple thread model.
    boost::asio::detail::mutex::scoped_lock lock(mutex_);
    
    if (closed_)
      return;

    closed_ = true;

    for (size_type i = 0; i < service_handlers_.size(); ++i)
      service_handlers_[i].reset();

    service_handlers_.clear();
  }
  
  /// Make a new handler.
  service_handler_type* make_handler(void)
  {
    return new service_handler_type(work_allocator().make_handler(),
        read_buffer_size_,
        io_timeout_);
  }  

  /// Push a handler into the pool.
  void push_handler(service_handler_type* handler_ptr)
  {
    BOOST_ASSERT(handler_ptr != 0);

    service_handler_ptr service_handler(handler_ptr,
          bind(&service_handler_pool::put_handler,
              shared_from_this(),
              _1));

    service_handlers_.push_back(service_handler);
  }

  /// Create handlers to the pool.
  void create_handler(size_type count)
  {
    for (size_type i = 0; i < count; ++i)
    {
      push_handler(make_handler());
      ++handler_count_;
    }
  }

  /// Get a handler from the pool.
  ///   Caller must check get_handler().get() != 0 for handler count have exceeded maximum.
  service_handler_ptr get_handler(void)
  {
    // Need lock in multiple thread model.
    boost::asio::detail::mutex::scoped_lock lock(mutex_);
  
    // Add new handler if the pool is in low water mark and handler count not exceed maximum.
    if (service_handlers_.size() <= pool_low_watermark_ && handler_count_ < pool_maximum_)
      create_handler(pool_increment_);

    // When handler count exceed limited, will return 
    service_handler_ptr service_handler;
    if (service_handlers_.size() > 0)
    {
      service_handler = service_handlers_.back();
      service_handlers_.pop_back();
    }

    return service_handler;
  }

private:
  /// Mutex to protect access to internal data.
  boost::asio::detail::mutex mutex_;

  /// Count of service_handler.
  size_type handler_count_;

  // Flag to indicate that the pool has been closed and all handlers need to be deleted.
  bool closed_;

  /// The pool of service_handler.
  std::vector<service_handler_ptr> service_handlers_;

  /// The allocator of work_handler.
  work_allocator_ptr work_allocator_;

  /// Preallocated handler number.
  size_type pool_init_size_;

  /// Low water mark of the pool.
  size_type pool_low_watermark_;

  /// High water mark of the pool.
  size_type pool_high_watermark_;

  /// Increase number of the pool.
  size_type pool_increment_;

  /// Maximum number of the pool.
  size_type pool_maximum_;

  /// The maximum size for asynchronous read operation buffer.
  size_type read_buffer_size_;

  /// The maximum size for asynchronous write operation buffer.
  size_type write_buffer_size_;

  /// The expiry seconds of session.
  unsigned int session_timeout_;

  /// The expiry seconds of io operation.
  unsigned int io_timeout_;
};

} // namespace bas

#endif // BAS_SERVICE_HANDLER_POOL_HPP
