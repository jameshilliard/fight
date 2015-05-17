#pragma  once

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include  <bas/io_service_pool.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>

class CTimeRecalculate
{
public:
	CTimeRecalculate();
	~CTimeRecalculate();
	void ReSetParam();
    unsigned int GetNextVidoeTic(unsigned int NowCapTic);
	unsigned int GetNextAudioTic(unsigned int NowCapAudioTic);
	void SetLastVideoTime(__int64 nLastVideoTime);
private: 
	unsigned int m_TimeServerVideoNow;//服务器转换的当前时间戳
	unsigned int m_TimeServerAudioNow;//服务器转换的当前时间戳

	unsigned int m_TimeLastCapVidoeTic;//采集端最后的视频时间戳

	unsigned int m_TimeLastCapAudioTic;//采集端最后的视频时间戳

	boost::asio::detail::mutex mutex_lock;	//登录过程锁
};