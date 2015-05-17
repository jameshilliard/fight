#pragma once
#include <boost/asio/detail/mutex.hpp>
#include <List>
template<class T>
class CMyList
{
public:
private:
	std::list<T> dDataBuf;
	boost::asio::detail::mutex mutex_;
public:
    void pushBack(const T& datBuf)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_);
		dDataBuf.push_back(datBuf);
	}
	void pushFront(const T& datBuf)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_);
		dDataBuf.push_front(datBuf);
	}
	T& GetData()
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_);
		return dDataBuf.front();
	}
	void clear()
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_);
		dDataBuf.clear();
	}
	bool DelData(T& t)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_);
		bool bresult = false;
		if (dDataBuf.size()>0)
		{
			t = dDataBuf.front();
			dDataBuf.pop_front();
			bresult = true;
		}
		return bresult;
		
	}
	bool IsHasData()
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_);
		if(dDataBuf.size()>0) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	int GetSize()
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_);
		return dDataBuf.size(); 
		
	}
};
