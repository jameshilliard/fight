//
// service_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Xu Ye Jun (moore.xu@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BAS_SERVICE_HANDLER_HPP
#define BAS_SERVICE_HANDLER_HPP

#include <boost/assert.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <bas/io_buffer.hpp>
#include <bas/Sendbuffer.hpp>
#include <tool/MyList.h>
extern "C" void  RealeaseSendBuf(SendBufPtr);

namespace bas {

/// Struct for deliver event cross multiple hander.
struct event
{
  /// Define type reference of std::size_t.
  typedef std::size_t size_type;

  enum state_t
  {
    none = 0,
    parent_open,
    parent_read,
    parent_write,
    parent_close,
    child_open,
    child_read,
    child_write,
    child_close,
    user = 1000
  };

  size_type state_;
  size_type value_;

  event(size_type state = 0,
      size_type value = 0)
    : state_(state),
      value_(value)
  {
  }
};

/// Object for handle socket asynchronous operations.
template<typename Work_Handler, typename Socket_Service = boost::asio::ip::tcp::socket>
class service_handler
  : public boost::enable_shared_from_this<service_handler<Work_Handler, Socket_Service> >,
    private boost::noncopyable
{
public:
  using boost::enable_shared_from_this<service_handler<Work_Handler, Socket_Service> >::shared_from_this;

  /// Define type reference of std::size_t.
  typedef std::size_t size_type;

  /// Define type reference of boost::asio::io_service.
  typedef boost::asio::io_service io_service_type;

  /// The type of the service_handler.
  typedef service_handler<Work_Handler, Socket_Service> service_handler_type;

  /// The type of the work_handler.
  typedef Work_Handler work_handler_type;

  /// The type of the socket that will be used to provide asynchronous operations.
  typedef Socket_Service socket_type;

  /// Construct a service_handler object.
  explicit service_handler(work_handler_type* work_handler,
      size_type read_buffer_size,
      size_type write_buffer_size = 0,
      unsigned int io_timeout = 0)
    : work_handler_(work_handler),
      socket_(),
      io_timer_(),
      io_service_(0),
      work_service_(0),
      stopped_(true),
      io_timeout_(io_timeout),
      read_buffer_(read_buffer_size)
  {
    // Make sure work_handler != 0.
    BOOST_ASSERT(work_handler != 0);
	m_bEmpty = true;
  }
  
  /// Destruct the service handler.
  ~service_handler()
  {
  }

  /// Get the io_buffer for incoming data.
  io_buffer& read_buffer()
  {
    return read_buffer_;
  }


  /// Get the io_service object used to perform asynchronous operations.
  io_service_type& io_service()
  {
    BOOST_ASSERT(io_service_ != 0);

    return *io_service_;
  }

  /// Get the io_service object used to dispatch synchronous works.
  io_service_type& work_service()
  {
    BOOST_ASSERT(work_service_ != 0);

    return *work_service_;
  }

  /// Get the socket associated with the service_handler.
  socket_type& socket()
  {
    BOOST_ASSERT(socket_.get() != 0);

    return *socket_;
  }

  /// Close the handler with error_code e from any thread.
  void close(const boost::system::error_code& e)
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    // Dispatch to io_service thread.
    io_service().dispatch(boost::bind(&service_handler_type::close_i,
        shared_from_this(),
        e));
  }

  /// Close the handler with error_code 0 from any thread.
  void close()
  {
     close(boost::system::error_code(0, boost::system::get_system_category()));
  }

  /// Start an asynchronous operation from any thread to read any amount of data from the socket.
  ///   Caller must be sure that read_buffer().space() > 0.
  void async_read_some()
  {
    if (read_buffer().space() == 0)
    {
      close(boost::system::error_code(boost::asio::error::no_buffer_space, boost::system::get_system_category()));

      return;
    }
    
    async_read_some(boost::asio::buffer(read_buffer().data() + read_buffer().size(), read_buffer().space()));
  }

  /// Start an asynchronous operation from any thread to read any amount of data to buffers from the socket.
  template<typename Buffers>
  void async_read_some(const Buffers& buffers)
  {
    io_service().dispatch(boost::bind(&service_handler_type::async_read_some_i<Buffers>,
        shared_from_this(),
        buffers));
  }

  /// Start an asynchronous operation from any thread to read a certain amount of data from the socket.
  ///   Caller must be sure that length != 0 and length <= read_buffer().space().
  void async_read(size_type length)
  {
    if (length == 0) 
    {
        return;
    }
	if(length > read_buffer().space())
	{
		read_buffer().recapacity(length + read_buffer().size());
	}
    async_read(boost::asio::buffer( read_buffer().data() + read_buffer().size()  , length));
  }

  /// Start an asynchronous operation from any thread to read a certain amount of data to buffers from the socket.
  template<typename Buffers>
  void async_read(const Buffers& buffers)
  {
    io_service().dispatch(boost::bind(&service_handler_type::async_read_i<Buffers>,
        shared_from_this(),
        buffers));
  }
  void async_send_normal(SendBufPtr sendBufPtr)
  {
	  if (!sendBufPtr)
		  return;
	  m_SendBuflist.pushBack(sendBufPtr);
	  async_send();
  }

  void async_send_now(SendBufPtr sendBufPtr)
  {
	  if (!sendBufPtr)
		  return;
	  m_SendBuflist.pushFront(sendBufPtr);
	  async_send();
  }

  void async_send()
  {
	  SendBufPtr sendBufPtr;
	  {
		  boost::asio::detail::mutex::scoped_lock lock(mutex_);
		  if (m_bEmpty)
		  {
			  bool bRet = m_SendBuflist.DelData(sendBufPtr);
			  if (bRet)
				  m_bEmpty = false;
			  else
				  return;
		  }
		  else
			  return;
	  }
	  async_write(boost::asio::buffer(sendBufPtr->Data(),sendBufPtr->length()),sendBufPtr);
  }


  /// Start an asynchronous operation from any thread to write buffers to the socket.
  template<typename Buffers>
  void async_write(const Buffers& buffers,SendBufPtr sendBufPtr)
  {
    io_service().dispatch(boost::bind(&service_handler_type::async_write_i<Buffers>,
        shared_from_this(),
        buffers,sendBufPtr));
  }

private:
  template<typename, typename, typename> friend class service_handler_pool;
  template<typename, typename, typename> friend class server;
  template<typename, typename, typename> friend class client;

  /// Bind a service_handler with the given io_service and work_service.
  template<typename Work_Allocator>
  void bind(io_service_type& io_service,
      io_service_type& work_service,
      Work_Allocator& work_allocator)
  {
    stopped_ = false;

    socket_.reset(work_allocator.make_socket(io_service));

    if (io_timeout_ != 0)
      io_timer_.reset(new boost::asio::deadline_timer(io_service));

    io_service_ = &io_service;
    work_service_ = &work_service;

    // Clear buffers for new operations.
    read_buffer().clear();
	m_SendBuflist.clear();
    // Clear work handler for new operations.
    //   Only low-level operations used to perform the necessary and should return ASAP.
    work_handler_->on_clear(shared_from_this());
  }

  /// Start an asynchronous connect, can be call from any thread.
  void connect(boost::asio::ip::tcp::endpoint& endpoint)
  {
    io_service().dispatch(boost::bind(&service_handler_type::connect_i,
        shared_from_this(),
        endpoint));
  }

  /// Start the first operation, can be call from any thread.
  void start()
  {
    BOOST_ASSERT(socket_.get() != 0);
    BOOST_ASSERT(io_service_ != 0);
    BOOST_ASSERT(work_service_ != 0);


    // Post to work_service for executing do_open.
    work_service().post(boost::bind(&service_handler_type::do_open,
        shared_from_this()));
  }

private:

  /// Start an asynchronous connect from io_service thread.
  void connect_i(boost::asio::ip::tcp::endpoint& endpoint)
  {
    BOOST_ASSERT(socket_.get() != 0);
    BOOST_ASSERT(io_service_ != 0);
    BOOST_ASSERT(work_service_ != 0);


    socket().lowest_layer().async_connect(endpoint,
        boost::bind(&service_handler_type::handle_connect,
            shared_from_this(),
            boost::asio::placeholders::error));
  }

  /// Start an asynchronous operation from io_service thread to read any amount of data to buffers from the socket.
  template<typename Buffers>
  void async_read_some_i(const Buffers& buffers)
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    // Set timer for io operation timeout.
    set_io_expiry();

    socket().async_read_some(buffers,
        boost::bind(&service_handler_type::handle_read,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
  }

  /// Start an asynchronous operation from io_service thread to read a certain amount of data to buffers from the socket.
  template<typename Buffers>
  void async_read_i(const Buffers& buffers)
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    // Set timer for io operation timeout.
    set_io_expiry();

    boost::asio::async_read(socket(),
        buffers,
        boost::bind(&service_handler_type::handle_read,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
  }

  /// Start an asynchronous operation from io_service thread to write buffers to the socket.
  template<typename Buffers>
  void async_write_i(const Buffers& buffers,SendBufPtr sendBufPtr)
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    // Set timer for io operation timeout.
    set_io_expiry();

    boost::asio::async_write(socket(),
        buffers,
        boost::bind(&service_handler_type::handle_write,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred,sendBufPtr));
  }

  /// Set timer for io operation timeout.
  void set_io_expiry(void)
  {
    if ((io_timeout_ == 0) || (io_timer_.get() == 0))
      return;

    io_timer_->expires_from_now(boost::posix_time::seconds(io_timeout_));
    io_timer_->async_wait(boost::bind(&service_handler_type::handle_timeout,
        shared_from_this(),
        boost::asio::placeholders::error));
  }

  /// Cancel timer for io operation timeout.
  void cancel_io_expiry(void)
  {
    if (io_timer_.get() != 0)
      io_timer_->cancel();
  }

  /// Handle completion of a connect operation in io_service thread.
  void handle_connect(const boost::system::error_code& e)
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    if (!e)
    {
      start();
    }
    else
    {
      // Close with error_code e.
      close_i(e);
    }
  }

  /// Handle completion of a read operation in io_service thread.
  void handle_read(const boost::system::error_code& e,
      size_type bytes_transferred)
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    if (!e)
    {
      // Post to work_service for executing do_read.
      work_service().post(boost::bind(&service_handler_type::do_read,
          shared_from_this(),
          bytes_transferred));
    }
    else
    {
      // Close with error_code e.
      close_i(e);
    }
  }

  /// Handle completion of a write operation in io_service thread.
  void handle_write(const boost::system::error_code& e,
      size_type bytes_transferred,SendBufPtr ptrSendBuf)
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    if (!e)
    {
		{
			boost::asio::detail::mutex::scoped_lock lock(mutex_);
			SendBufPtr sendBufPtr;
			m_SendBuflist.DelData(sendBufPtr);
			if (sendBufPtr)
			{
				m_bEmpty = false;
				async_write(boost::asio::buffer(sendBufPtr->Data(),sendBufPtr->length()),sendBufPtr);
			}
			else
			{
				m_bEmpty = true;

			}
		}
		RealeaseSendBuf(ptrSendBuf);
		work_service().post(boost::bind(&service_handler_type::do_write,
			shared_from_this(),
			bytes_transferred));
    }
    else
    {
      // Close with error_code e.
      close_i(e);
    }
  }

  /// Handle timeout of whole operation in io_service thread.
  void handle_timeout(const boost::system::error_code& e)
  {
    // The handler has been stopped or timer has been cancelled, do nothing.
    if (stopped_ || e == boost::asio::error::operation_aborted)
      return;

    if (!e)
    {
      // Close with error_code boost::system::timed_out.
      close_i(boost::system::error_code(boost::asio::error::timed_out, boost::system::get_system_category()));
    }
    else
    {
      // Close with error_code e.
      close_i(e);
    }
  }

  /// Close the handler in io_service thread.
  void close_i(const boost::system::error_code& e)
  {
    if (!stopped_)
    {
      stopped_ = true;

      // Initiate graceful service_handler closure.
      boost::system::error_code ignored_ec;
      socket().lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
      socket().lowest_layer().close(ignored_ec);

      cancel_io_expiry();

	  m_SendBuflist.clear();
      // Post to work_service to executing do_close.
      work_service().post(boost::bind(&service_handler_type::do_close,
          shared_from_this(),
          e));
    }
  }

  /// Do on_open in work_service thread.
  void do_open()
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    // Call on_open function of the work handler.
	m_gStrIp = socket_->remote_endpoint().address().to_string();
	m_gnPort = socket_->remote_endpoint().port();
    work_handler_->on_open(shared_from_this());
	m_bEmpty = true;
  }

  /// Do on_read in work_service thread.
  void do_read(size_type bytes_transferred)
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    // Call on_read function of the work handler.
	read_buffer().produce(bytes_transferred);
    work_handler_->on_read(shared_from_this(), bytes_transferred);
  }

  /// Do on_write in work_service thread.
  void do_write(size_type bytes_transferred )
  {
    // The handler has been stopped, do nothing.
    if (stopped_)
      return;

    // Call on_write function of the work handler.
    work_handler_->on_write(shared_from_this(), bytes_transferred);
  }

 
  

  /// Do on_close and reset handler for next connaction in work_service thread.
  void do_close(const boost::system::error_code& e)
  {
    // Call on_close function of the work handler.
    work_handler_->on_close(shared_from_this(), e);
    io_timer_.reset();
  }
private:
  typedef boost::shared_ptr<work_handler_type> work_handler_ptr;
  typedef boost::shared_ptr<socket_type> socket_ptr;
  typedef boost::shared_ptr<boost::asio::deadline_timer> timer_ptr;

  /// Work handler of the service_handler.
  work_handler_ptr work_handler_;

  /// Socket for the service_handler.
  socket_ptr socket_;

 

  /// Timer for io operation timeout.
  timer_ptr io_timer_;

  /// The expiry seconds of io operation.
  unsigned int io_timeout_;

  /// The io_service for for asynchronous operations.
  io_service_type* io_service_;

  /// The io_service for for executing synchronous works.
  io_service_type* work_service_;

  // Flag to indicate that the handler has been stopped and can not do synchronous operations.
  bool stopped_;

  /// Buffer for incoming data.
  io_buffer read_buffer_;

  private:
	  CMyList<SendBufPtr> m_SendBuflist;
	  bool m_bEmpty;
	  boost::asio::detail::mutex mutex_;
	
	  
  public:
	  std::string  m_gStrIp;
	  unsigned int m_gnPort;
};

} // namespace bas

#endif // BAS_SERVICE_HANDLER_HPP
