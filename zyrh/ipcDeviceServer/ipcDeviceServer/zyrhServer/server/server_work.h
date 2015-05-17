//
// server_work.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2009 Xu Ye Jun (moore.xu@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BAS_ECHO_SERVER_WORK_HPP
#define BAS_ECHO_SERVER_WORK_HPP
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <bas/service_handler.hpp>
#include <iostream>
#include <boost/enable_shared_from_this.hpp>

class server_work : public boost::enable_shared_from_this<server_work>
{
public:
	typedef bas::service_handler<server_work> server_handler_type;
	typedef boost::shared_ptr<server_handler_type> handler_ptr;
	typedef boost::weak_ptr<server_handler_type>   handler_wPtr;
	server_work()
	{

	}
	virtual ~server_work()
	{

	}
	virtual void on_clear(handler_ptr handler) = 0;
	virtual void on_open(handler_ptr handler) = 0;
	virtual void on_read(handler_ptr handler, std::size_t bytes_transferred) = 0;
	virtual void on_write(handler_ptr handler, std::size_t bytes_transferred) = 0;
	virtual void on_close(handler_ptr handler, const boost::system::error_code& e) = 0;
	virtual void on_parent(handler_ptr handler, const bas::event event) = 0;
	virtual void on_child(handler_ptr handler, const bas::event event) = 0;

	virtual void SendData(handler_ptr handler,char* pBuf,unsigned int nBufLen);
	//rtmp 
	virtual void CapLogin(std::string rmtpUr,handler_ptr handle){};
	virtual void ViewLogin(std::string rmtpUr,handler_ptr handle){};
	virtual void RecordLogin(std::string rtmpUrl,handler_ptr handle){};
protected:
	handler_wPtr m_handle_wPtr;
};
typedef bas::service_handler<server_work> server_handler_type;
typedef boost::shared_ptr<server_handler_type> handler_ptr;
typedef boost::weak_ptr<server_handler_type>   handler_wPtr;
typedef boost::shared_ptr<server_work>   server_work_ptr;
#endif // BAS_ECHO_SERVER_WORK_HPP
