//
// server_work.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2009 Xu Ye Jun (moore.xu@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BAS_ECHO_HTTP_SERVER_WORK_HPP
#define BAS_ECHO_HTTP_SERVER_WORK_HPP
#include <server_work.h>
#include <boost/shared_ptr.hpp>
#include <devsdk/DevSdk.h>


class http_server_work:server_work
{
public:

	http_server_work();
	~http_server_work();
	void on_clear(handler_ptr handler);
	void on_open(handler_ptr handler);
	void on_read(handler_ptr handler, std::size_t bytes_transferred);
	void on_write(handler_ptr handler, std::size_t bytes_transferred);
	void on_close(handler_ptr handler, const boost::system::error_code& e);
	void on_parent(handler_ptr handler, const bas::event event);
	void on_child(handler_ptr handler, const bas::event event);
private:
    CSendbuffer m_httpRecvBuf;
	unsigned int  ResquestHttpHead(handler_ptr handler,unsigned int httpCode,unsigned int nConntentLength);
	unsigned int  ResquestHttpHeadAndData(handler_ptr handler,unsigned int httpCode,const std::string sHttpData,bool bIsNeedConvertUtf8 = true);

	void CheckM3U8FIle(handler_ptr handler,int &nCounts,boost::shared_ptr<CdevSdk> pDev);
	
	void SendData(handler_ptr handler,const char* pBuf,unsigned int nBufLen);
	unsigned int  GetPostConnectLength(char* pBuf,unsigned int nLen);
	bool HandleHead(handler_ptr handler);
	void HandleHttpCmd(handler_ptr handler,char* buf, unsigned int buflen);
	void HandleHttpRequest(handler_ptr handler,char* buf, unsigned int buflen);

	unsigned int m_nContentLen;//Content内容长度
	unsigned int m_nHeadSize;  //包头的大小

	//文件操作的相关属性
	unsigned int m_nAllSendSize; //要发送的大小
	unsigned int m_nBeenSendSize;//已发送的大小
	FILE* m_HttpFile;
	std::string m_strHttpPath;
	std::string m_strHttpParam;

	std::string m_sHttpType;

	typedef boost::shared_ptr<boost::asio::deadline_timer> timer_ptr;
	timer_ptr m_io_timer_Ptr;
	boost::asio::io_service* m_pIoService;
};
typedef boost::shared_ptr<http_server_work> http_server_work_Ptr;
#endif // BAS_ECHO_HTTP_SERVER_WORK_HPP
