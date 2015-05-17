#include "stdafx.h"
#include "TimeRecalculate.h"

CTimeRecalculate::CTimeRecalculate()
{
	ReSetParam();

}
CTimeRecalculate::~CTimeRecalculate()
{

}
void CTimeRecalculate::ReSetParam()
{
	m_TimeServerVideoNow = 0;
	m_TimeLastCapVidoeTic = 0;
	m_TimeLastCapAudioTic = 0;
	m_TimeServerAudioNow = 0;
}
unsigned int  CTimeRecalculate::GetNextVidoeTic(unsigned int NowCapVdioeTic)
{
	//
	boost::asio::detail::mutex::scoped_lock lock(mutex_lock);
	unsigned int retTic = 0;

	if (m_TimeServerVideoNow == 0)
	{
		m_TimeServerVideoNow = 0;
	}
	else
	{
		if(NowCapVdioeTic <= m_TimeLastCapVidoeTic || NowCapVdioeTic > m_TimeLastCapVidoeTic + 1000)
		{
			m_TimeServerVideoNow += 50;
		}
		else
		{
			m_TimeServerVideoNow +=  (NowCapVdioeTic - m_TimeLastCapVidoeTic);
		}	
	}
	m_TimeLastCapVidoeTic = NowCapVdioeTic;
	retTic = m_TimeServerVideoNow;
	if (m_TimeServerVideoNow < m_TimeServerAudioNow)
	{
		retTic =  m_TimeServerAudioNow + 50;
	}
	retTic = m_TimeServerAudioNow = m_TimeServerVideoNow ;
	return retTic;
}
unsigned int  CTimeRecalculate::GetNextAudioTic(unsigned int NowCapAudioTic)
{
	//
	boost::asio::detail::mutex::scoped_lock lock(mutex_lock);
	unsigned int retTic = 0;
	if (m_TimeServerAudioNow == 0)
	{
		m_TimeServerAudioNow = 0;//初始化时间戳从100开始
	}
	else
	{
		if(NowCapAudioTic <= m_TimeLastCapAudioTic || NowCapAudioTic > m_TimeLastCapAudioTic + 10000)
		{
			m_TimeServerAudioNow += 50;
		}
		else
		{
			m_TimeServerAudioNow +=  (NowCapAudioTic - m_TimeLastCapAudioTic);
		}	
	}
	m_TimeLastCapAudioTic = NowCapAudioTic;
	retTic = m_TimeServerAudioNow;
	if (m_TimeServerAudioNow < m_TimeServerVideoNow)
	{
		retTic = m_TimeServerVideoNow + 50;
	}
	return retTic;
}
void CTimeRecalculate::SetLastVideoTime(__int64 nLastVideoTime)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_lock);
	m_TimeServerVideoNow = nLastVideoTime;

}