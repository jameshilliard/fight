#include "DevManager.h"
#include "DevSdk.h"
#include <GlobalParam.h>
#include <GlobalFun.h>
#include "xml/XMLMethod.h"
#include "httpClient/httpClient.h"


std::string GetDevIdkey(std::string sDevId,int nchannel);

CDevManager::CDevManager()
{
	char buf[2048];
	int i=1000; 

	char FilePath[MAX_PATH+1]= {0};
	char *p = NULL;

	GetModuleFileNameA(NULL, FilePath, sizeof(FilePath)); //获取程序当前执行文件名
	p=strrchr(FilePath, '\\');                       
	*p='\0';
	GetCurrentDirectoryA(1000,buf);   //得到当前工作路径 
	m_configPath = FilePath;
	m_configPath += "/config.ini";

	memset(buf,0,sizeof(buf));
	GetPrivateProfileString("ZyrhOpenService","DeviceInfoHttpAddr","",buf,2048,m_configPath.c_str());
	m_DeviceInfoHttpAddr=buf;

	memset(buf,0,sizeof(buf));
	GetPrivateProfileString("ZyrhOpenService","ServerStopStreamType","",buf,2048,m_configPath.c_str());
	m_serverStopStreamType=atoi(buf);

	memset(buf,0,sizeof(buf));
	GetPrivateProfileString("ZyrhOpenService","StopStreamTimeOut","",buf,2048,m_configPath.c_str());
	m_stopStreamTimeOut=atoi(buf);

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

void CDevManager::decodeDeviceInfo(std::string deviceInfoString)
{
	TiXmlDocument doc;
	doc.Parse(deviceInfoString);
	doc.SaveFile("C:\\error.xml");
	if ( doc.Error() )
	{
		doc.SaveFile("C:\\error.xml");
		CString str;
		str.Format("Error in %s: %s\n", doc.Value(), doc.ErrorDesc());
		return FALSE;
	}
	TiXmlElement *lmtRoot = doc.RootElement();
	if(!lmtRoot)
		return FALSE;


	TiXmlElement *lmtId = lmtRoot->FirstChildElement("ArrayOfZyrhDeviceInfo");


	if (lmtId)
	{
		
		TiXmlElement *lmtParamRoot = lmtRoot->FirstChildElement("ZyrhDeviceInfo");

		if (lmtParamRoot)
		{
			CString sKey;
			CString sVaule;
			TiXmlElement *lmtTmp = lmtParamRoot->FirstChildElement();
			while (lmtTmp)
			{
				TiXmlElement *lmtKey = lmtTmp->FirstChildElement("key");
				if (lmtKey)
				{
					TiXmlElement *lmtValue = lmtKey->NextSiblingElement("value");
					if (lmtValue)
					{
						sKey.Format("%s",lmtKey->GetText());
						sVaule.Format("%s",lmtValue->GetText());
						sVaule = sVaule=="(null)"?"":sVaule;
						sData->params[sKey] = sVaule;
					}
				}

				lmtTmp = lmtTmp->NextSiblingElement();
			}
		}
	}
	return TRUE;
	
}

void CDevManager::StartUpateDeviceInfo()
{
	CHTTPClient sHttpClient;
	while(1)
	{
		std::string strRet="";
		sHttpClient.HttpPost(m_DeviceInfoHttpAddr,"strDevCodeList=",strlen("strDevCodeList="),strRet);
		if(!strRet.empty())
		{
			decodeDeviceInfo(strRet);	
		}
		Sleep(1000);
	}
	

}
void CDevManager::UpateDeviceInfo()
{
	m_UpateDeviceInfoThread.StartThread(boost::bind(&CDevManager::StartUpateDeviceInfo,this));
}