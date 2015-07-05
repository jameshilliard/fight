#include "DevManager.h"
#include "DevSdk.h"
#include <GlobalParam.h>
#include <GlobalFun.h>
#include "httpClient/httpClient.h"
#include "xml/tinyxml/tinyxml.h"
#include <Map>


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

int CDevManager::AddNewDev(CdevSdkParam mCdevSdkParam)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_);
	if (!GetDev(mCdevSdkParam.m_sDevId,mCdevSdkParam.m_CdevChannelDeviceParam.m_nChannelNo))
	{
		//if(mCdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort!=8012)
		//	return -1;
		boost::shared_ptr<CdevSdk> devPtr(new CdevSdk);
		mCdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort=m_nRtspServerStartPort;
		m_DevList.AddData(GetDevIdkey(mCdevSdkParam.m_sDevId,mCdevSdkParam.m_CdevChannelDeviceParam.m_nChannelNo),devPtr);
		m_DevListIndex.AddData(devPtr->m_nIndex,devPtr);
		mCdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort=m_nRtspServerStartPort;
		m_nRtspServerStartPort++;
		
		return devPtr->StartDev(mCdevSdkParam);
	}
	else
	{
		boost::shared_ptr<CdevSdk> devPtr=GetDev(mCdevSdkParam.m_sDevId,mCdevSdkParam.m_CdevChannelDeviceParam.m_nChannelNo);	
		mCdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort=0;
		if(!devPtr->m_CdevSdkParam.isEqual(mCdevSdkParam))
			return devPtr->StartDev(mCdevSdkParam);
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

BOOL CDevManager::decodeDeviceInfo(std::string deviceInfoString,CdevSdkParam &mCdevSdkParam,std::string strOnlineDevice)
{
	TiXmlDocument doc;
	doc.Parse((const char *)deviceInfoString.c_str());
	doc.SaveFile("C:\\error.xml");
	if ( doc.Error() )
	{
		doc.SaveFile("C:\\error.xml");
		return FALSE;
	}
	TiXmlElement *lmtRoot = doc.RootElement();
	if(!lmtRoot)
		return FALSE;
	std::vector<CdevSdkParam> vectorCdevSdkParam;
	TiXmlElement *curDeviceInfo = lmtRoot->FirstChildElement("ZyrhDeviceInfo");
	while(curDeviceInfo)
	{
		TiXmlElement *devCodeElement=curDeviceInfo->FirstChildElement("DevCode");
		if(devCodeElement)
			mCdevSdkParam.m_sDevId=devCodeElement->FirstChild()->Value();

		TiXmlElement *ChannelQuantityElement=curDeviceInfo->FirstChildElement("ChannelQuantity");
		if(ChannelQuantityElement)
			mCdevSdkParam.m_nChannelQuantity=atoi(ChannelQuantityElement->FirstChild()->Value());

		TiXmlElement *ServerLineCodeElement=curDeviceInfo->FirstChildElement("ServerLineCode");
		if(ServerLineCodeElement)
			mCdevSdkParam.m_nDevLine=atoi(ServerLineCodeElement->FirstChild()->Value());

		TiXmlElement *AudioTypeElement=curDeviceInfo->FirstChildElement("AudioType");
		if(AudioTypeElement)
			mCdevSdkParam.m_nAudioType=atoi(AudioTypeElement->FirstChild()->Value());

		TiXmlElement *LimitedPreviewTimeElement=curDeviceInfo->FirstChildElement("LimitedPreviewTime");
		if(LimitedPreviewTimeElement)
			mCdevSdkParam.m_nLimitedPreviewTime=atoi(LimitedPreviewTimeElement->FirstChild()->Value());
		
		TiXmlElement *ChannelListElement=curDeviceInfo->FirstChildElement("ChannelList");
		TiXmlElement *ChanneElement=NULL;
		if(ChannelListElement)
			ChanneElement=ChannelListElement->FirstChildElement("ZyrhChannelInfo");
		while(ChanneElement)
		{
			TiXmlElement *ChannelNoElement=ChanneElement->FirstChildElement("ChannelNo");
			if(ChannelNoElement)
				mCdevSdkParam.m_CdevChannelDeviceParam.m_nChannelNo=atoi(ChannelNoElement->FirstChild()->Value());

			TiXmlElement *ConnectTypeElement=ChanneElement->FirstChildElement("ConnectType");
			if(ConnectTypeElement)
				mCdevSdkParam.m_CdevChannelDeviceParam.m_nConnectType=atoi(ConnectTypeElement->FirstChild()->Value());

			TiXmlElement *ChannelNameElement=ChanneElement->FirstChildElement("ChannelName");
			if(ConnectTypeElement)
				mCdevSdkParam.m_CdevChannelDeviceParam.m_sChannelName=ChannelNameElement->FirstChild()->Value();
			
			TiXmlElement *StreamTypeElement=ChanneElement->FirstChildElement("StreamType");
			if(StreamTypeElement)
				mCdevSdkParam.m_CdevChannelDeviceParam.m_nStreamType=atoi(StreamTypeElement->FirstChild()->Value());

			TiXmlElement *PlatLinkagElement=ChanneElement->FirstChildElement("PlatLinkag");
			if(PlatLinkagElement)
			{
				TiXmlElement *PlatDevNameElement=PlatLinkagElement->FirstChildElement("PlatDevName");
				if(PlatDevNameElement)
					mCdevSdkParam.m_CdevChannelDeviceParam.m_sPlatDevName=PlatDevNameElement->FirstChild()->Value();

				TiXmlElement *PlatDevPwdElement=PlatLinkagElement->FirstChildElement("PlatDevPwd");
				if(PlatDevPwdElement)
					mCdevSdkParam.m_CdevChannelDeviceParam.m_sPlatDevPwd=PlatDevPwdElement->FirstChild()->Value();

				TiXmlElement *PlatDevPortElement=PlatLinkagElement->FirstChildElement("PlatDevPort");
				if(PlatDevPortElement)
					mCdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort=atoi(PlatDevPortElement->FirstChild()->Value());
			}
			if(strOnlineDevice.find(mCdevSdkParam.m_sDevId)!=string::npos)
				mCdevSdkParam.m_isOnline=1;
			else
				mCdevSdkParam.m_isOnline=0;
			mCdevSdkParam.m_isChannelEnable=1;
			AddNewDev(mCdevSdkParam);
			vectorCdevSdkParam.push_back(mCdevSdkParam);
			Sleep(100);
			ChanneElement=ChanneElement->NextSiblingElement("ZyrhChannelInfo");
		}
		curDeviceInfo=curDeviceInfo->NextSiblingElement();
	}
	if(m_vectorCdevSdkParam.size()!=0)
	{
		vector<CdevSdkParam>::iterator lastIt;
		vector<CdevSdkParam>::iterator nowIt;
		int findFlag=1;
		for (lastIt=m_vectorCdevSdkParam.begin() ; lastIt < m_vectorCdevSdkParam.end(); lastIt++ )
		{
			findFlag=0;
			for (nowIt=vectorCdevSdkParam.begin(); nowIt<vectorCdevSdkParam.end(); nowIt++ )
			{	
				if(lastIt->m_sDevId==nowIt->m_sDevId && lastIt->m_CdevChannelDeviceParam.m_nChannelNo==nowIt->m_CdevChannelDeviceParam.m_nChannelNo)
				{
					findFlag=1;
				}
			}	
			if(findFlag==0)
			{
				lastIt->m_isChannelEnable=0;
				AddNewDev(*lastIt);	
			}
		}	
	}
	m_vectorCdevSdkParam=vectorCdevSdkParam;
	return TRUE;
}

void CDevManager::StartUpateDeviceInfo()
{
	char buf[2048]={0};
	CHTTPClient sHttpClient;
	std::string str,str1,str2;
	int iWebPort=0;
	char FilePath[MAX_PATH+1]= {0};
	char *p = NULL;

	GetModuleFileNameA(NULL, FilePath, sizeof(FilePath)); //获取程序当前执行文件名
	p=strrchr(FilePath, '\\');                       
	*p='\0';

	m_configPath = FilePath;
	m_configPath += "/config.ini";

	memset(buf,0,sizeof(buf));
	GetPrivateProfileString("ZyrhOpenService","ServerStopStreamType","",buf,2048,m_configPath.c_str());
	m_serverStopStreamType=atoi(buf);

	memset(buf,0,sizeof(buf));
	GetPrivateProfileString("ZyrhOpenService","StopStreamTimeOut","",buf,2048,m_configPath.c_str());
	m_stopStreamTimeOut=atoi(buf);

	memset(buf,0,sizeof(buf));
	GetPrivateProfileString("ZyrhOpenService","RtspServerPort","",buf,2048,m_configPath.c_str());
	int iRet=sscanf(buf,"%d-%d",&m_nRtspServerStartPort,&m_nRtspServerStopPort);
	if(iRet!=2)
	{
		g_logger.TraceInfo("RtspServerPort设置异常");
	}
	m_bDevManagerStart=true;
	GetPrivateProfileString("ZyrhOpenService","Sdkclient","",buf,2048,m_configPath.c_str());
	str = buf;

	CdevSdkParam sCdevSdkParam;
	GetPrivateProfileString("ZyrhOpenService","LocalIpaddr","",buf,2048,m_configPath.c_str());
	sCdevSdkParam.m_CdevChannelDeviceParam.m_sLocalIpaddr = buf;

	GetPrivateProfileString("ZyrhOpenService","DaHuaBeatEnable","",buf,2048,m_configPath.c_str());
	sCdevSdkParam.m_beatEnable = atoi(buf);
	
	for (int i = 1;i<=6;i++)
	{
		strseparate((char*)str.c_str(),str1,str2,"_");
		switch(i)
		{
		case 1:
			sCdevSdkParam.m_sSdkServerIp = str1;
			break;
		case 2:
			sCdevSdkParam.m_nSdkServerPort = atoi(str1.c_str());
			break;
		case 3:
			sCdevSdkParam.m_nServerLine = atoi(str1.c_str());
			break;
		case 4:
			sCdevSdkParam.m_sUserName = str1;
			break;
		case 5:
			sCdevSdkParam.m_spassword = str1;
			break;
		case 6:
			iWebPort = atoi(str1.c_str());
			break;
		default:
			break;
		}
		str = str2;
	}
	std::string strDeviceInfoHttpAddr="";
	std::string strOnlineDeviceCodeHttpAddr="";
	sCdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort=m_nRtspServerStartPort;
	sCdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStopPort=m_nRtspServerStopPort;
	memset(buf,0,sizeof(buf));
	sprintf(buf,"http://%s:%d/ws/zyrh.asmx/GetDevicePlatLinkageConfig",sCdevSdkParam.m_sSdkServerIp.c_str(),iWebPort);
	strDeviceInfoHttpAddr.append(buf);

	memset(buf,0,sizeof(buf));
	sprintf(buf,"http://%s:%d/ws/zyrh.asmx/GetOnlineDeviceCode",sCdevSdkParam.m_sSdkServerIp.c_str(),iWebPort);
	strOnlineDeviceCodeHttpAddr.append(buf);
	//http://60.12.220.24:81/ws/zyrh.asmx/GetOnlineDeviceCode  m_OnlineDeviceCodeHttpAddr

	g_logger.TraceInfo("plserver start,version is %s",SOFT_VERSION);
	bool bRet=false;
	while(m_bDevManagerStart)
	{
		std::string strOnlineDeviceRet="";
		bRet=sHttpClient.HttpPost(strOnlineDeviceCodeHttpAddr,"",0,strOnlineDeviceRet);
		if(!strOnlineDeviceRet.empty() && bRet)
		{
			std::string strRet="";
			bRet=sHttpClient.HttpPost(strDeviceInfoHttpAddr,"strDevCodeList=",strlen("strDevCodeList="),strRet);
			if(!strRet.empty() && bRet)
			{
				//if(strOnlineDeviceRet.find("string")!=string::npos)
					//printf("---%s---\n",strOnlineDeviceRet.c_str());
				decodeDeviceInfo(strRet,sCdevSdkParam,strOnlineDeviceRet);
				g_logger.TraceInfo("StartUpateDeviceInfo success");
				Sleep(30000);
			}
			else
			{
				g_logger.TraceInfo("read %s",strDeviceInfoHttpAddr.c_str());
				Sleep(2000);
			}
		}
		else
		{
			g_logger.TraceInfo("read %s",strDeviceInfoHttpAddr.c_str());
			Sleep(2000);
		}
	}
}
void CDevManager::UpateDeviceInfo()
{
	m_bDevManagerStart=true;
	m_UpateDeviceInfoThread.StartThread(boost::bind(&CDevManager::StartUpateDeviceInfo,this));	
}