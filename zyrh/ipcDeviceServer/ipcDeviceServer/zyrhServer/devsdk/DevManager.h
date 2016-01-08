#pragma once
#include "stdafx.h"
#include <publicHead.h>
#include "Thread.h"
#include <utils/app.h>


class CdevSdk;
class CdevSdkParam;
class CDevManager :
	public App
{
public:
	CDevManager();
	~CDevManager();
	virtual bool Init();
	virtual void Uninit();
	int AddNewDev(std::string sDevID,unsigned int nDevLine,unsigned int nChannel,int nType,std::string sUrl = "");
	int AddNewDev(CdevSdkParam mCdevSdkParam);
	boost::shared_ptr<CdevSdk> GetDev(std::string sDevId,int nChannel);
	boost::shared_ptr<CdevSdk> GetDev(unsigned int nindex);
	void DelDev(unsigned int nindex);
	void DelDev(std::string sDevId,int nChannel);
	void DelDevTimeOut(std::string sDevId,int nChannel);
	void StartCheckTimeOut();
	void CheckDevTimeOut();
	void StartUpateDeviceInfo();
	BOOL decodeDeviceInfo(std::string deviceInfoString,CdevSdkParam &mCdevSdkParam,std::string strOnlineDevice);

private:
	CMyMap<std::string,boost::shared_ptr<CdevSdk>> m_DevList;
	CMyMap2<boost::shared_ptr<CdevSdk>> m_DevListIndex;
	boost::asio::detail::mutex mutex_;
	CThread m_Thread;
	CThread m_UpateDeviceInfoThread;
	std::string m_configPath;
	std::vector<CdevSdkParam> m_vectorCdevSdkParam;
	//#模拟IPC服务器停止流类型0:http停流接口无效,改为超时时间 1:http停流接口有效
	unsigned int m_serverStopStreamType;
	//#模拟IPC服务器停流时间单位分钟
	unsigned int m_stopStreamTimeOut;
	unsigned int m_nRtspServerStartPort;
	unsigned int m_nRtspServerStopPort;
	bool m_bDevManagerStart;
};