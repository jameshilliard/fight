#pragma once
#include "stdafx.h"
#include <publicHead.h>
#include "Thread.h"
class CdevSdk;
class CdevSdkParam;
class CDevManager
{
public:
	CDevManager();
	~CDevManager();
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
	void UpateDeviceInfo();
	BOOL decodeDeviceInfo(std::string deviceInfoString,CdevSdkParam &mCdevSdkParam);


private:
	CMyMap<std::string,boost::shared_ptr<CdevSdk>> m_DevList;
	CMyMap2<boost::shared_ptr<CdevSdk>> m_DevListIndex;
	boost::asio::detail::mutex mutex_;
	CThread m_Thread;
	CThread m_UpateDeviceInfoThread;
	std::string m_configPath;
	std::string m_DeviceInfoHttpAddr;
	//#ģ��IPC������ֹͣ������0:httpͣ���ӿ���Ч,��Ϊ��ʱʱ�� 1:httpͣ���ӿ���Ч
	unsigned int m_serverStopStreamType;
	//#ģ��IPC������ͣ��ʱ�䵥λ����
	unsigned int m_stopStreamTimeOut;
	unsigned int m_nRtspServerStartPort;
	bool m_bDevManagerStart;
};