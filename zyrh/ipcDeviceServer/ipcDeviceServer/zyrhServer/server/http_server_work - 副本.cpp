#include "http_server_work.h"
#include <GlobalParam.h>
#include <GlobalClass.h>
#include <GlobalFun.h>
#include "devsdk/DevManager.h"
#include "devsdk/DevSdk.h"
#include "../Logger.h"
#include <json/json.h>
#include "httpClient/httpClient.h"

http_server_work::http_server_work()
{
	m_nBeenSendSize = 0;
	m_nAllSendSize = 0;
	m_nContentLen = 0;
	m_nHeadSize = 0;
	m_HttpFile = NULL;
	
	m_httpRecvBuf.clear();
	boost::asio::io_service& _IoService = CGlobalClass::GetInstance()->GetIoservice()->get_io_service();

	m_io_timer_Ptr.reset(new boost::asio::deadline_timer(_IoService) );
	m_pIoService = &_IoService;

}
http_server_work::~http_server_work()
{
	
}
void http_server_work::on_clear(handler_ptr handler)
{
	m_nBeenSendSize = 0;
	m_nAllSendSize = 0;
	m_nContentLen = 0;
	m_nHeadSize = 0;
	m_HttpFile = NULL;
	m_httpRecvBuf.clear();
	m_strHttpPath = "";
	m_strHttpParam = "";
	m_sHttpType = "";
}

void http_server_work::on_open(handler_ptr handler)
{
	handler->async_read_some();		
}

void http_server_work::on_read(handler_ptr handler, std::size_t bytes_transferred)
{
	m_httpRecvBuf.InsertData((char*)handler->read_buffer().data(),bytes_transferred);
	handler->read_buffer().clear();
	int datalen = m_httpRecvBuf.length();
	char* pStart  = (char*)m_httpRecvBuf.Data();
	char* pEnd = pStart + datalen;

	char* op;
	op  = strtok(pStart, (" \r\n"));
	if (op == NULL)
	{
		handler->async_read_some();
		return;
	}
	m_sHttpType = op;
	if(m_sHttpType == GET)
	{
		int nEnd = *(int*)(pEnd - 4);
		if (nEnd == 0x0A0D0A0D )
		{
			m_httpRecvBuf.push_back((unsigned char)'\0');
			g_logger.TraceInfo("%s",m_httpRecvBuf.Data());
			FILE* file = fopen("G:\1.txt","ab+");
			fwrite(m_httpRecvBuf.Data(),1,m_httpRecvBuf.length(),file);
			fclose(file);
			if(!HandleHead(handler))
			{
				return;
			}
			HandleHttpRequest(handler,NULL,0);
			return;

		}
		else
		{
			handler->async_read_some();
			return;
		}
	}
	else if (m_sHttpType == POST)
	{
		if (m_nHeadSize == 0)
		{
			char* pCur = pStart;
			for(pCur = pStart;pCur < pEnd - 4;pCur++)
			{
				int nEnd = *(int*)(pCur - 4);
				if (nEnd == 0x0A0D0A0D )
				{
					m_nHeadSize = pCur - pStart;
					m_nContentLen = GetPostConnectLength(pStart + 5,pEnd - pStart);
					if (!m_nContentLen)
					{
						return;
					}
					if(!HandleHead(handler))
					{
						return;
					}
				}

			}
		}
		if (m_nContentLen + m_nHeadSize == m_httpRecvBuf.length() )
		{
			m_httpRecvBuf.push_back('\0');
			FILE* file = fopen("g://1.txt","ab+");
			fwrite(m_httpRecvBuf.Data(),1,m_httpRecvBuf.length(),file);
			fclose(file);
			HandleHttpRequest(handler,(char*)m_httpRecvBuf.Data() + m_nHeadSize,m_nContentLen);
			return;
		}
	}
	handler->async_read_some();
}

void http_server_work::on_write(handler_ptr handler, std::size_t bytes_transferred)
{
	m_nBeenSendSize += bytes_transferred;
	if (m_nBeenSendSize < m_nAllSendSize)
	{
		if (m_HttpFile != NULL)
		{
			char buf[1024*10];
			unsigned int NReadSize = fread(buf,1,1024*10,m_HttpFile);
			if (NReadSize > 0)
			{
				SendData(handler,buf,NReadSize);
			}
			else
			{
				g_logger.TraceInfo("NReadSize == 0 %d m_nAllSendSize:%d port:%d ",ftell(m_HttpFile),m_nAllSendSize,handler->m_gnPort);
			}
		}
	}
	else
	{
		handler->close();
	}


}

void http_server_work::on_close(handler_ptr handler, const boost::system::error_code& e)
{

	switch (e.value())
	{
		// Operation successfully completed.
	case 0:
	case boost::asio::error::eof:
		//std::cout << "close ok ...............\n";
		//std::cout.flush();
		break;

		// Connection breaked.
	case boost::asio::error::connection_aborted:
	case boost::asio::error::connection_reset:
	case boost::asio::error::connection_refused:
		break;

		// Other error.
	case boost::asio::error::timed_out:

		break;

	case boost::asio::error::no_buffer_space:
	default:
		break;
	}
	if (m_HttpFile != NULL)
	{
		fclose(m_HttpFile);
		m_HttpFile = NULL;
	}
	m_strHttpParam = "";
}

void http_server_work::on_parent(handler_ptr handler, const bas::event event)
{

}

void http_server_work::on_child(handler_ptr handler, const bas::event event)
{

} 

unsigned int  http_server_work::ResquestHttpHead(handler_ptr handler,unsigned int httpCode,unsigned int nConntentLength)
{
	char httpHead[1024];
	std::string strhttpHead;
	std::string httpCodeString = GetCodeString(httpCode);
	sprintf(httpHead, ("HTTP/1.1 %d %s\r\n"),httpCode,httpCodeString.c_str());
	strhttpHead = httpHead;

	sprintf(httpHead,  ("Server: DWK/1.1\r\n"));
	strhttpHead += httpHead;

	sprintf(httpHead,  ("Pragma: no-cache\r\n"));
	strhttpHead += httpHead;

	sprintf(httpHead,  ("Cache-control: no-cache\r\n"));
	strhttpHead += httpHead;

	sprintf(httpHead,  ("Content-Length: %d\r\n"),nConntentLength);
	strhttpHead += httpHead;

	std::string sContentType = GetConnectType(m_strHttpPath);
	sprintf(httpHead,  ("Content-Type: %s\r\n"),sContentType.c_str());
	strhttpHead += httpHead;

	sprintf(httpHead,  ("\r\n"));
	strhttpHead += httpHead;

	SendData(handler,strhttpHead.c_str(),strhttpHead.length());
	m_nAllSendSize = nConntentLength + strhttpHead.length();
	return strhttpHead.length();
}
unsigned int  http_server_work::ResquestHttpHeadAndData(handler_ptr handler,unsigned int httpCode,const std::string sHttpData,bool bIsNeedConvertUtf8)
{
	std::string strHttpSendData = sHttpData;
	if (bIsNeedConvertUtf8)
	{
		strHttpSendData = GBKToUTF8(strHttpSendData);
	}
	char httpHead[1024];
	std::string strhttpHead;
	std::string httpCodeString = GetCodeString(httpCode);
	sprintf(httpHead, ("HTTP/1.1 %d %s\r\n"),httpCode,httpCodeString.c_str());
	strhttpHead = httpHead;

	sprintf(httpHead,  ("Server: DWK/1.1\r\n"));
	strhttpHead += httpHead;

	sprintf(httpHead,  ("Pragma: no-cache\r\n"));
	strhttpHead += httpHead;

	sprintf(httpHead,  ("Cache-control: no-cache\r\n"));
	strhttpHead += httpHead;

	sprintf(httpHead,  ("Content-Length: %d\r\n"),strHttpSendData.length());
	strhttpHead += httpHead;

	std::string sContentType = GetConnectType(m_strHttpPath);
	sprintf(httpHead,  ("Content-Type: %s\r\n"),sContentType.c_str());
	strhttpHead += httpHead;

	sprintf(httpHead,  ("\r\n"));
	strhttpHead += httpHead;

	strhttpHead += strHttpSendData;

	SendData(handler,strhttpHead.c_str(),strhttpHead.length());
	m_nAllSendSize = strhttpHead.length();
	return strhttpHead.length();
}
void http_server_work::SendData(handler_ptr handler,const char* pBuf,unsigned int nBufLen)
{
	SendBufPtr sendBufPtr = CGlobalClass::GetInstance()->GetSendBuf();
	if (sendBufPtr)
	{
		sendBufPtr->InsertData((char*)pBuf,nBufLen);
		handler->async_send_normal(sendBufPtr);
	}
}
unsigned int http_server_work::GetPostConnectLength(char* pBuf,unsigned int nLen)
{
	if (pBuf != NULL)
	{
		char* pConnectLenth = strstr(pBuf,"Content-Length: ");
		if (pConnectLenth != NULL)
		{
			pConnectLenth =  strtok(pConnectLenth,"\r\n");
			if (pConnectLenth != NULL)
			{
				int nCntentlen = strlen("Content-Length: ");
				if (strlen(pConnectLenth) > nCntentlen)
				{
					return atoi(pConnectLenth + nCntentlen);
				}	
			}
		}
	}
	return 0;
}

bool http_server_work::HandleHead(handler_ptr handler)
{
	char* pStart = (char*)m_httpRecvBuf.Data() + m_sHttpType.length() + 1;
	std::string fullUrl = g_strtok(pStart," HTTP");
	if(fullUrl == "")
	{
		handler->close();
		return false;
	}
	m_strHttpPath = fullUrl;
	char*  sParam = strstr((char*)fullUrl.c_str(),"?");
	if (sParam != NULL&&*(sParam+1) != '\0')
	{
		sParam++;
		m_strHttpParam = sParam;
		m_strHttpPath = g_strtok((char*)fullUrl.c_str(),"?");
	}
	if (m_strHttpPath == "")
	{
		handler->close();
		return false;
	}
	return true;

}
void http_server_work::HandleHttpRequest(handler_ptr handler,char* buf, unsigned int buflen)
{
	if (m_strHttpPath.find("Cmd") != std::string::npos )
	{
		HandleHttpCmd(handler,buf,buflen);
		return;
	}
	if (m_sHttpType == GET)
	{
		std::string strFileName = CGlobalParam::GetInstance()->m_HttpWorkDir + m_strHttpPath;

		m_httpRecvBuf.clear();
		m_HttpFile = fopen(strFileName.c_str(),"rb");
		if (m_HttpFile)//文件存在
		{
			fseek( m_HttpFile, 0L, SEEK_END );
			unsigned int nFileSize = ftell(m_HttpFile);
			fseek( m_HttpFile, 0L, SEEK_SET );
			m_nBeenSendSize = 0;
			ResquestHttpHead(handler,200,nFileSize);
		}
		else//文件不存在
		{
			char retStr[2048];
			sprintf(retStr,"%s in not exist %d",m_strHttpPath.c_str(),GetTickCount());
			unsigned int nLen = strlen(retStr);
			ResquestHttpHeadAndData(handler,404,retStr);
		}

	}
}
void http_server_work::HandleHttpCmd(handler_ptr handler,char* buf, unsigned int buflen)
{
	if (m_strHttpParam == ""&&buf)
	{
		m_strHttpParam = buf;
	}
	map<std::string,std::string> sMapParam;
	
	if (m_strHttpPath == "/CmdStartRtmp")
	{
		std::string str = m_strHttpParam;
		//JosnDeCoder(str,sMapParam);
		//g_logger.TraceInfo("param:%s",m_strHttpParam.c_str());
		if (sMapParam.size() == 0)
		{	
			httpParamToMap(m_strHttpParam,sMapParam);
		}
		std::string sDevid = sMapParam["devid"];
		unsigned int nChannel = atoi(sMapParam["channel"].c_str());
		unsigned int nDevLine = atoi(sMapParam["devline"].c_str());
		std::string sRtmpUrl = sMapParam["url"];
		char bufUrl[2048],bufTmp[1024];
		ZeroMemory(bufUrl,2048);
		ZeroMemory(bufTmp,1024);
		sprintf(bufTmp,"%s%02d",sDevid.c_str(),nChannel);
		g_logger.TraceInfo("sRtmpUrl:%s bufTmp:%s",sRtmpUrl.c_str(),bufTmp);
		strcpy(bufUrl,sRtmpUrl.c_str());
		char* pStart = strstr(bufUrl,bufTmp);
		*(pStart + strlen(bufTmp)) = '\0';
		//std::size_t nindex = sRtmpUrl.find_first_of(bufTmp,0,strlen(bufTmp));
		g_logger.TraceInfo(" bufTmp:%s bufUrl:%s" ,bufTmp,bufUrl);
		sRtmpUrl = bufUrl;
		int ret = CGlobalClass::GetInstance()->GetDevManager()->AddNewDev(sDevid,nDevLine,nChannel,0x01,sRtmpUrl);	
		char strMsg[2048];
		map<std::string,std::string> mapRet;
		if (ret < 0)
		{
			mapRet["retsult"] = "101";
			sprintf(strMsg,"设备取流失败 ret %d ",ret);
		}
		else
		{
			mapRet["retsult"] = "100";
			sprintf(strMsg,"设备取流成功 ret %d ",ret);
		}
		mapRet["msg"] = strMsg;
		std::string retStr;
		JosnEnCoder(mapRet,retStr);
		ResquestHttpHeadAndData(handler,200,retStr);
	}
	if (m_strHttpPath == "/CmdStopRtmp")
	{
		std::string str = m_strHttpParam;
		JosnDeCoder(str,sMapParam);
		if (sMapParam.size() == 0)
		{
			httpParamToMap(m_strHttpParam,sMapParam);
		}
		std::string sDevid = sMapParam["devid"];
		unsigned int nChannel = atoi(sMapParam["channel"].c_str());
		boost::shared_ptr<CdevSdk> pDevPtr = CGlobalClass::GetInstance()->GetDevManager()->GetDev(sDevid,nChannel);
		if (pDevPtr)
		{
			if(CGlobalParam::GetInstance()->m_bStopStream)
			{
				pDevPtr->m_nType = pDevPtr->m_nType&0x02;
				CGlobalClass::GetInstance()->GetDevManager()->DelDev(sDevid,nChannel);
			}
			else
			{
				pDevPtr->m_nCheckDelTimeOut = time(NULL);
			}	
		}
		map<std::string,std::string> mapRet;
		char strMsg[2048];
		sprintf(strMsg,"停止rtmp成功 ");
		mapRet["retsult"] = "100";
		mapRet["msg"] = strMsg;
		std::string retStr;
		JosnEnCoder(mapRet,retStr);
		ResquestHttpHeadAndData(handler,200,retStr);
	}
	else if (m_strHttpPath == "/live/CmdGet.m3u8")
	{
		httpParamToMap(m_strHttpParam,sMapParam);
		std::string sDevid = sMapParam["devid"];
		unsigned int nChannel = atoi(sMapParam["channel"].c_str());
		unsigned int nDevLine = atoi(sMapParam["devline"].c_str());
		boost::shared_ptr<CdevSdk> pDevPtr = CGlobalClass::GetInstance()->GetDevManager()->GetDev(sDevid,nChannel);
		if (!pDevPtr)
		{
			CGlobalClass::GetInstance()->GetDevManager()->AddNewDev(sDevid,nDevLine,nChannel,0x02);	
		}
		pDevPtr = CGlobalClass::GetInstance()->GetDevManager()->GetDev(sDevid,nChannel);
		if (pDevPtr)
		{
			std::string strRet = pDevPtr->CreateM3u8File();
			int nCounts = 1;
			if (strRet != "")
			{
				ResquestHttpHeadAndData(handler,200,strRet);
			}
			else
			{
				CheckM3U8FIle(handler,nCounts,pDevPtr);
			}
		}
	}
	else if (m_strHttpPath == "/CmdGetDevList")
	{
		 CHTTPClient sHttpClient;
		 std::string strUrl = "http://lt.3gvs.net:81/api/device.aspx?";
		 strUrl += m_strHttpParam;
		 std::string strRet;
		 sHttpClient.HttpGet(strUrl.c_str(),strRet);
		 ResquestHttpHeadAndData(handler,200,strRet,false);
	}
	else if (m_strHttpPath == "/CmdPtzControl")
	{
		std::string sDevid = sMapParam["devid"];
		unsigned int nChannel = atoi(sMapParam["channel"].c_str());
		int ptz_cmd = atoi(sMapParam["ptzcmd"].c_str());
		int action = atoi(sMapParam["action"].c_str());
		int param = atoi(sMapParam["param"].c_str());

		std::string sMsg;
		int  nRet = -1;
		boost::shared_ptr<CdevSdk> pDevPtr = CGlobalClass::GetInstance()->GetDevManager()->GetDev(sDevid,nChannel);
		if (pDevPtr)
		{
			nRet = pDevPtr->CmdPtzControl(sDevid,nChannel,ptz_cmd,action,param,sMsg);
		}
		else
		{
			sMsg = "未开始观看流";
		}
		map<std::string,std::string> mapRet;
		char bufstr[256];
		sprintf(bufstr,"%d",nRet);

		mapRet["retsult"] =  bufstr;
		mapRet["msg"] = sMsg;
		std::string retStr;
		JosnEnCoder(mapRet,retStr);
		ResquestHttpHeadAndData(handler,200,retStr);
	}
}

void http_server_work::CheckM3U8FIle(handler_ptr handler,int &nCounts,boost::shared_ptr<CdevSdk> pDevPtr)
{
	if (nCounts >= 20)
	{
		return;
	}
	std::string strRet = pDevPtr->CreateM3u8File();
	if (strRet != "")
	{
		ResquestHttpHeadAndData(handler,200,strRet);
		return;
	}
	
	if (m_io_timer_Ptr)
	{
		nCounts++;
		m_io_timer_Ptr->expires_from_now(boost::posix_time::seconds(1));
		m_io_timer_Ptr->async_wait(boost::bind(&http_server_work::CheckM3U8FIle,
			this,handler,nCounts,pDevPtr ));
	}
}