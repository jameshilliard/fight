#include "StdAfx.h"
#include "M3U8List.h"
#include <GlobalParam.h>
#include <GlobalFun.h>
#include "x264/H264FrameParser.h"

const std::string strdir = "live/";

CM3u8List::CM3u8List()
{
	ReSetParam();
	m_nTargetDuration = 7;
	m_pTsCur = new CTsEncoder;
}
CM3u8List::~CM3u8List()
{
	if (m_pTsCur)
	{
		m_pTsCur->CloseFile();
		delete m_pTsCur;
		m_pTsCur = NULL;
	}

}
void CM3u8List::ReSetParam()
{
	m_nBeginTic = 100;
	m_nLastTic = 0;
	m_nCreateTic = 0;
	m_pTsCur = 0;
	m_nSequence = 1;
}
void CM3u8List::handleVideo(uint8_t* buf,int size,__int64 nowTimeStamp,bool bkey)
{
	if (m_sDir == "")
	{
		m_sDir = CGlobalParam::GetInstance()->m_HttpWorkDir + strdir;
		CreatDir((char*)m_sDir.c_str());
	}
	__int64 nowTs = m_TimeRecal.GetNextVidoeTic(nowTimeStamp);

	double fileduration;
	if (bkey)
	{
		if (size < 2000)
		{
			g_logger.TraceInfo("key size bkey:%d",size);
		}
	}
	if (IsNeedCreateFile(nowTs,fileduration,bkey))
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
		if (m_pTsCur->IsOpen())
		{
			std::string str = m_pTsCur->GetFileName().c_str();
			m_pTsCur->CloseFile();			

			fileduration = fileduration/1000;
			TsFileBuf sTsFile;
			sTsFile.fileName = str;
			sTsFile.duration = fileduration;	
			UpdateM3u8List(sTsFile);
		}
		m_pTsCur->OpenFile(m_sDir,CreatTsName());
		 
	}
	if (m_pTsCur->IsOpen())
	{
		//printf(" nowTs:%d \n",nowTs);
		m_pTsCur->handleVideo(buf,size,nowTs,bkey);
	}
	else
	{
		//g_logger.TraceInfo("m_pTsCur->IsOpen() flase bkey:%d",bkey);
	}

}
void CM3u8List::handleAudio(uint8_t* data,int size,__int64 nowTimeStamp)
{
	__int64 prevTimeStamp;
	__int64 nowTs = m_TimeRecal.GetNextAudioTic(nowTimeStamp);

	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if (m_pTsCur)
	{
		m_pTsCur->handleAudio((uint8_t*)data,size,nowTs);
	}

}

void CM3u8List::UpdateM3u8List(TsFileBuf& tsFileBuf)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	m_vcTsList.push_back(tsFileBuf);
	if (m_vcTsList.size() > 3)
	{
		TsFileBuf _TsFileBuf = m_vcTsList.front();
		m_vcTsList.erase(m_vcTsList.begin());
		m_vcTsDelList.push_back(_TsFileBuf);
		if (m_vcTsDelList.size() > 3)
		{
			TsFileBuf _TsFileBuf = m_vcTsDelList.front();
			m_vcTsDelList.erase(m_vcTsDelList.begin());
			std::string fulltsName = m_sDir + _TsFileBuf.fileName;
			g_logger.TraceInfo("del %s",fulltsName.c_str());
			remove(fulltsName.c_str());
		}
	}
	m_nSequence++;
	if (m_vcTsList.size() == 0)
	{
		printf("Start Live %s \n",m_sdveid.c_str());
	}
// 	FILE* file = fopen("D:/live/live.m3u8.tmp","ab+");
// 	std::string str = CreateM3u8File();
// 	fwrite(str.c_str(),1,str.length(),file);
// 	fclose(file);
// 	remove("D:/live/live.m3u8");
// 	rename("D:/live/live.m3u8.tmp","D:/live/live.m3u8");

}
bool CM3u8List::IsNeedCreateFile(__int64 nowTimeStamp,double& fileduration,bool bkey)
{
	bool bret = false;
	if (bkey)
	{
		fileduration = nowTimeStamp - m_nCreateTic;
		if (m_nCreateTic == 0 || fileduration > m_nTargetDuration*1000)
		{
			m_nCreateTic = nowTimeStamp;
			bret = true;
		}
	}
	return bret;
}
std::string CM3u8List::CreateM3u8File()
{
	std::string strRet;
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if (m_vcTsList.size() == 0)
	{
		return strRet;
	}
	char bufTmp[2048];;
	sprintf(bufTmp,"#EXTM3U\r\n");
	strRet = bufTmp;

	sprintf(bufTmp,"#EXT-X-VERSION:3\r\n");
	strRet += bufTmp;

	sprintf(bufTmp,"#EXT-X-TARGETDURATION:%d\r\n",10);
	strRet += bufTmp;

	sprintf(bufTmp,"#EXT-X-MEDIA-SEQUENCE:%d\r\n",m_nSequence);
	strRet += bufTmp;

	vector<TsFileBuf>::iterator it = m_vcTsList.begin();
	while(it != m_vcTsList.end())
	{
		sprintf(bufTmp, "#EXTINF:%.3f,\r\n", it->duration);
		strRet += bufTmp;

		sprintf(bufTmp, "%s\r\n",it->fileName.c_str());
		strRet += bufTmp;
		it++;
	}
	return strRet;
}
std::string CM3u8List::CreatTsName()
{
	std::string TsFileName;
	time_t curTime;
	struct tm * pTimeInfo = NULL;
	time(&curTime);
	pTimeInfo = localtime(&curTime);
	char temp[MAX_STR_LEN] = {0};
	sprintf(temp, "%s_%04d_%02d_%02d_%02d_%02d_%02d.ts",m_sdveid.c_str(),pTimeInfo->tm_year+1900,pTimeInfo->tm_mon+1,pTimeInfo->tm_mday, pTimeInfo->tm_hour, pTimeInfo->tm_min, pTimeInfo->tm_sec);
	TsFileName = temp;
	return TsFileName;
}
