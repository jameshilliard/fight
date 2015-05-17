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
	unsigned int m_TimeServerVideoNow;//������ת���ĵ�ǰʱ���
	unsigned int m_TimeServerAudioNow;//������ת���ĵ�ǰʱ���

	unsigned int m_TimeLastCapVidoeTic;//�ɼ���������Ƶʱ���

	unsigned int m_TimeLastCapAudioTic;//�ɼ���������Ƶʱ���

	boost::asio::detail::mutex mutex_lock;	//��¼������
};