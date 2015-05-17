#pragma once 

#include <string>

class SDKServerData
{
public:

	SDKServerData()
	{
		m_sSdkServerIp="";
		m_nSdkServerPort=0;
		m_spassword="";
		m_sUserName="";
		m_nServerLine=0;

		m_nDevLine=0;
		m_nnchannel=0;
		m_sDevId="";
	}

	SDKServerData(std::string sSdkServerIp,unsigned int nSdkServerPort,std::string sPassword,
						std::string sUserName,unsigned int nServerLine,unsigned int nDevLine,
						unsigned int nChannel,std::string sDevId)
	{
		m_sSdkServerIp=sSdkServerIp;
		m_nSdkServerPort=nSdkServerPort;
		m_spassword=sPassword;
		m_sUserName=sUserName;
		m_nServerLine=nServerLine;

		m_nDevLine=nDevLine;
		m_nnchannel=nChannel;
		m_sDevId=sDevId;
	};
public:	
	//sdk пео╒
	std::string 	m_sSdkServerIp;
	unsigned int 	m_nSdkServerPort;
	std::string	 	m_spassword;
	std::string 	m_sUserName;
	unsigned int 	m_nServerLine;

	unsigned int 	m_nDevLine;
	unsigned int 	m_nnchannel;
	std::string 	m_sDevId;
	
};

