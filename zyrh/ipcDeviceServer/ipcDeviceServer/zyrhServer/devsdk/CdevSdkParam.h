#pragma once 
#include <string>

class CdevChannelDeviceParam
{
public:
	CdevChannelDeviceParam()
	{
		m_nChannelNo=0;
		m_sChannelName="";
		m_nConnectType=0;
		m_nStreamType=0;
		m_sPlatDevName="";
		m_sPlatDevPwd="";
		m_nPlatDevPort=0;
		m_nRtspServerStartPort=0;
		m_nRtspServerStopPort=0;
	}

	bool CdevChannelDeviceParam::isEqual(CdevChannelDeviceParam mCdevChannelDeviceParam)
	{
		if(m_nChannelNo!=mCdevChannelDeviceParam.m_nChannelNo)
			return false;
		if(m_sChannelName!=mCdevChannelDeviceParam.m_sChannelName)
			return false;
		if(m_nConnectType!=mCdevChannelDeviceParam.m_nConnectType)
			return false;
		if(m_nStreamType!=mCdevChannelDeviceParam.m_nStreamType)
			return false;
		if(m_sPlatDevName!=mCdevChannelDeviceParam.m_sPlatDevName)
			return false;
		if(m_sPlatDevPwd!=mCdevChannelDeviceParam.m_sPlatDevPwd)
			return false;
		if(m_nPlatDevPort!=mCdevChannelDeviceParam.m_nPlatDevPort)
			return false;
		return true;
	}
	unsigned int m_nChannelNo;
	std::string  m_sChannelName;
	unsigned int m_nConnectType;
	unsigned int m_nStreamType;
	std::string m_sPlatDevName;
	std::string m_sPlatDevPwd;
	unsigned int m_nPlatDevPort;
	unsigned int m_nRtspServerStartPort;
	unsigned int m_nRtspServerStopPort;
};

class CdevSdkParam
{
public:

	CdevSdkParam()
	{
		m_sSdkServerIp="";
		m_nSdkServerPort=0;
		m_spassword="";
		m_sUserName="";
		m_nServerLine=0;

		m_nDevLine=0;
		m_devNo=0;
		m_sDevId="";

		m_nChannelQuantity=0;
		m_nAudioType=0;
		m_nLimitedPreviewTime=0;
	}

	bool CdevSdkParam::isEqual(CdevSdkParam mCdevSdkParam)
	{
		if(m_nServerLine!=mCdevSdkParam.m_nServerLine)
			return false;
		if(m_nDevLine!=mCdevSdkParam.m_nDevLine)
			return false;
		if(m_sDevId!=mCdevSdkParam.m_sDevId)
			return false;		
		if(m_nChannelQuantity!=mCdevSdkParam.m_nChannelQuantity)
			return false;
		if(m_nAudioType!=mCdevSdkParam.m_nAudioType)
			return false;		
		if(m_nLimitedPreviewTime!=mCdevSdkParam.m_nLimitedPreviewTime)
			return false;
		return m_CdevChannelDeviceParam.isEqual(mCdevSdkParam.m_CdevChannelDeviceParam);
	}

	CdevSdkParam(std::string sSdkServerIp,unsigned int nSdkServerPort,std::string sPassword,
		std::string sUserName,unsigned int nServerLine,unsigned int nDevLine,std::string sDevId)
	{
		m_sSdkServerIp=sSdkServerIp;
		m_nSdkServerPort=nSdkServerPort;
		m_spassword=sPassword;
		m_sUserName=sUserName;
		m_nServerLine=nServerLine;

		m_nDevLine=nDevLine;
		m_sDevId=sDevId;

	};

public:	
	//sdk пео╒
	unsigned int    m_devNo;
	std::string 	m_sSdkServerIp;
	unsigned int 	m_nSdkServerPort;
	std::string	 	m_spassword;
	std::string 	m_sUserName;
	unsigned int 	m_nServerLine;

	unsigned int 	m_nDevLine;
	std::string 	m_sDevId;

	unsigned int    m_nChannelQuantity;
	unsigned int    m_nAudioType;
	unsigned int    m_nLimitedPreviewTime;

	CdevChannelDeviceParam m_CdevChannelDeviceParam;
};
