//
// io_buffer.hpp
// ~~~~~~~~~~~~~
//
// The class based on boost::asio::detail::buffered_stream_storage.
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Copyright (c) 2009 Xu Ye Jun (moore.xu@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BAS_SEND_BUFFER_HPP
#define BAS_SEND_BUFFER_HPP

#include <boost/assert.hpp>
#include <boost/asio/detail/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <vector>
using namespace std;


class CSendbuffer 
{
public:
	CSendbuffer()
	{

	}
	~CSendbuffer()
	{

	}
	CSendbuffer(char* sendBuf,unsigned int buflen)
	{
		buffer_.assign(sendBuf,sendBuf + buflen);
	}
	void InsertData(char* sendBuf,unsigned int buflen)
	{
		buffer_.insert(buffer_.end(),sendBuf,sendBuf + buflen);
	}
	void push_back(char chValue)
	{
		buffer_.push_back(chValue);
	}
	char* Data()
	{
		if (buffer_.size() > 0)
		{
			return &buffer_[0];
		}
		return NULL;
	}
	unsigned int length()
	{
		return buffer_.size();
	}
	void clear()
	{
		buffer_.clear();
	}

private:
  std::vector<char> buffer_;
};
typedef boost::shared_ptr<CSendbuffer> SendBufPtr;

class CSendBufList
{
public:
	CSendBufList()
	{

	}
	~CSendBufList()
	{

	}

	SendBufPtr GetSendBuf()
	{
		boost::asio::detail::mutex::scoped_lock lock(_mutex);
		if (m_SendBuflist.size() < 200)
		{
			for (int i = 0;i<300;i++)
			{
				SendBufPtr SendBuf(new CSendbuffer);
				m_SendBuflist.push_back(SendBuf);
			}	
		}
		SendBufPtr retSendBuf  =  m_SendBuflist.back();
		m_SendBuflist.pop_back();
		return retSendBuf;

	}
	void RealeaseSendBuf(SendBufPtr sendBuf)
	{
		boost::asio::detail::mutex::scoped_lock lock(_mutex);
		sendBuf->clear();
		m_SendBuflist.push_back(sendBuf);

	}
private:
	list<SendBufPtr> m_SendBuflist;
	boost::asio::detail::mutex _mutex;	
};
#endif 
