#pragma once
#include <boost/asio/detail/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <map>
using namespace std;

 template <class V>
 class CMyMap2
 {
 public:
 	CMyMap2(void)
	{
	}
 	~CMyMap2(void)
	{
	}

 	unsigned int  AddData(unsigned int& nIndex  ,V sValue)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		static unsigned int g_index = 0;
		while(1)
		{
			g_index++;
			if (m_DataMap.find(g_index) == m_DataMap.end())
			{
				break;
			}
		}
		nIndex = g_index;
		m_DataMap[g_index] = sValue;
		return nIndex;
	}

 	bool GetData(unsigned int nIndex,V &sValue)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		map<unsigned int,V>::iterator it = m_DataMap.find(nIndex);
		if (it != m_DataMap.end())
		{
			sValue = it->second;
			return true;
		}
		else
			return false;
	}
	bool IsHasData(unsigned int nIndex)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		map<unsigned int,V>::iterator it = m_DataMap.find(nIndex);
		if (it != m_DataMap.end())
		{
			return true;
		}
		else
			return false;
	}

	bool GetBeginData(V &sValue)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		map<unsigned int,V>::iterator it = m_DataMap.begin();
		if (it != m_DataMap.end())
		{
			sValue = it->second;
			return true;
		}
		else
			return false;
	}

	bool RemoveHead(V &sValue)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		map<unsigned int,V>::iterator it = m_DataMap.begin();
		if (it != m_DataMap.end())
		{
			sValue = it->second;
			m_DataMap.erase(it);
			return true;
		}
		else
			return false;
	}

 	bool DelData(unsigned int tKey)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		bool bRet = false;
		map<unsigned int,V>::iterator it = m_DataMap.find(tKey);
		if (it != m_DataMap.end())
		{
			m_DataMap.erase(it);
			bRet = true;
		}
		return bRet;
	}
	bool DelDataAndGetValue(unsigned int tKey,V &sValue)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		map<unsigned int,V>::iterator it = m_DataMap.find(tKey);
		if (it != m_DataMap.end())
		{
			sValue = it->second;
			m_DataMap.erase(it);
			return true;
		}
		else
		{
			return false;
		}
	}

 	void DelAllData()
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		m_DataMap.clear();
	}

 	map<unsigned int,V> GetDataList()
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		return m_DataMap;
	}

	void SetDataList(map<unsigned int,V> mDataList)
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		m_DataMap = mDataList;
	}

	int GetListSize()
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Map);
		return m_DataMap.size();
	}
 private:
 	boost::asio::detail::mutex mutex_Map;
 	map<unsigned int ,V> m_DataMap;
 };