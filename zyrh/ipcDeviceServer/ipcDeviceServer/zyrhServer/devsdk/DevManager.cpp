#include "DevManager.h"
#include "DevSdk.h"
#include <GlobalParam.h>
#include <GlobalFun.h>
std::string GetDevIdkey(std::string sDevId,int nchannel);
CDevManager::CDevManager()
{

}
CDevManager::~CDevManager()
{

}
int CDevManager::AddNewDev(std::string sDevID,unsigned int nDevLine,unsigned int nChannel,int nType,std::string sUrl)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_);
	if (!GetDev(sDevID,nChannel))
	{
		boost::shared_ptr<CdevSdk> devPtr(new CdevSdk);
		m_DevList.AddData(GetDevIdkey(sDevID,nChannel),devPtr);
		m_DevListIndex.AddData(devPtr->m_nIndex,devPtr);
		devPtr->m_srtmpurl = sUrl;
		return devPtr->StartDev((char*)CGlobalParam::GetInstance()->m_SdkServerIp.c_str(),
			CGlobalParam::GetInstance()->m_nSdkServerPort,
			CGlobalParam::GetInstance()->m_nSdkServerLine,
			(char*)sDevID.c_str(),"admin",nDevLine,nChannel,nType);
	}
	return 0;
	
}
boost::shared_ptr<CdevSdk> CDevManager::GetDev(std::string sDevID,int nChannel)
{
	boost::shared_ptr<CdevSdk> devPtr;
	m_DevList.GetData(GetDevIdkey(sDevID,nChannel),devPtr);
	return devPtr;
}
boost::shared_ptr<CdevSdk> CDevManager::GetDev(unsigned int nindex)
{
	boost::shared_ptr<CdevSdk> devPtr;
	m_DevListIndex.GetData(nindex,devPtr);
	return devPtr;
}
void CDevManager::DelDev(unsigned int nindex)
{
	m_DevListIndex.DelData(nindex);
}
void CDevManager::DelDev(std::string sDevId,int nChannel)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_);
	boost::shared_ptr<CdevSdk> DevsdkPtr = GetDev(sDevId,nChannel);
	if (DevsdkPtr)
	{
		m_DevList.DelData(GetDevIdkey(sDevId,nChannel));
		m_DevListIndex.DelData(DevsdkPtr->m_nIndex);
		DevsdkPtr->StopDev();
	}
	
}
void CDevManager::DelDevTimeOut(std::string sDevId,int nChannel)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_);
	boost::shared_ptr<CdevSdk> DevsdkPtr = GetDev(sDevId,nChannel);
	if (DevsdkPtr)
	{
		DevsdkPtr->m_nCheckDelTimeOut = time(NULL);
	}

}

std::string GetDevIdkey(std::string sDevId,int nchannel)
{
	std::string strRet;
	char buf[256];
	sprintf(buf,"%s_%d",sDevId.c_str(),nchannel);
	strRet = buf;
	return strRet;
}
void CDevManager::CheckDevTimeOut()
{
	map<std::string,boost::shared_ptr<CdevSdk>> mapDevList;
	mapDevList = m_DevList.GetDataList();
	map<std::string,boost::shared_ptr<CdevSdk>>::iterator it = mapDevList.begin();
	while(it != mapDevList.end())
	{
		if (it->second)
		{
			if (it->second->m_nCheckDelTimeOut  != 0 &&time(NULL) - it->second->m_nCheckDelTimeOut >CGlobalParam::GetInstance()->m_bStopStreamTimeOut*60)
			{
				DelDev(it->second->m_sDevId,it->second->m_nnchannel);
			}
		}
		it++;
	}

}
void CDevManager::StartCheckTimeOut()
{
	m_Thread.StartThread(boost::bind(&CDevManager::CheckDevTimeOut,this));
}