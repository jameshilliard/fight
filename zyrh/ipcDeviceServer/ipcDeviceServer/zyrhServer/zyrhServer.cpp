// RtmpServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CrashRpt/CrashRpt.h"
//
// win_main_server.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//#define BOOST_LIB_DIAGNOSTIC
#include <iostream>
#include <string>
#include "devsdk/wmpclient.h"
#include "./GlobalHttpFun.h"
#include <GlobalParam.h>
#include <GlobalClass.h>
#include "devsdk/DevManager.h"

#if 0
#include <bas/server.hpp>
#include <bas/service_handler.hpp>
#include <boost/thread.hpp>
#include <server_work_allocator.hpp>
#include "server/http_server_work_allocator.hpp"
typedef bas::server<server_work, server_work_allocator> server;
typedef bas::service_handler_pool<server_work, server_work_allocator> service_handler_pool_type;

void StartServer()
{
	char buf[256];
	
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	(_tcsrchr(buf,'\\'))[1] = 0;
	SetCurrentDirectoryA(buf);
	if(!initCrash())
	{
		g_logger.TraceInfo("initCrash false,程序将退出");
		Sleep(10*1000);
		exit(1);
	}
	WMP_Init();
	std::string path = buf;
	path += "config.ini";

	GetPrivateProfileString("zyrhRtmpServer","httpDir","",buf,2048,path.c_str());
	CGlobalParam::GetInstance()->m_HttpWorkDir = buf;

	unsigned int port = GetPrivateProfileInt("zyrhRtmpServer","httpPort",0,path.c_str());

	GetPrivateProfileString("zyrhRtmpServer","sdkServerIp","",buf,2048,path.c_str());
	CGlobalParam::GetInstance()->m_SdkServerIp = buf;


	CGlobalParam::GetInstance()->m_nSdkServerPort = GetPrivateProfileInt("zyrhRtmpServer","sdkServerPort",0,path.c_str());
	CGlobalParam::GetInstance()->m_nSdkServerLine = GetPrivateProfileInt("zyrhRtmpServer","sdkServerLine",0,path.c_str());


	GetPrivateProfileString("zyrhRtmpServer","rtmpServerIp","",buf,2048,path.c_str());
	CGlobalParam::GetInstance()->m_RtmpServerIp = buf;

	CGlobalParam::GetInstance()->m_nRtmpkServerPort = GetPrivateProfileInt("zyrhRtmpServer","rtmpServerPort",0,path.c_str());

	CGlobalParam::GetInstance()->m_bStopStream = GetPrivateProfileInt("zyrhRtmpServer","ServerStopStreamType",0,path.c_str());

	CGlobalParam::GetInstance()->m_bStopStreamTimeOut = GetPrivateProfileInt("zyrhRtmpServer","StopStreamTimeOut",0,path.c_str());

	std::size_t io_pool_size = 4;
	std::size_t work_pool_init_size =  8;
	std::size_t work_pool_high_watermark = 32;
	std::size_t work_pool_thread_load = 5000;
	std::size_t accept_queue_length =  1000;
	std::size_t preallocated_handler_number = 1000;
	std::size_t read_buffer_size = 10000;
	unsigned int session_timeout = 0;
	unsigned int io_timeout = 60;

	boost::shared_ptr<server> httpServer(new server(new service_handler_pool_type((server_work_allocator*)new http_server_work_allocator(),
		preallocated_handler_number,
		read_buffer_size,
		0,
		session_timeout,
		io_timeout),
		"0.0.0.0",
		port,
		io_pool_size,
		work_pool_init_size,
		work_pool_high_watermark,
		work_pool_thread_load,
		accept_queue_length)) ;
	g_logger.TraceInfo("开启http服务器:%d http目录:%s",port,CGlobalParam::GetInstance()->m_HttpWorkDir.c_str());

	g_logger.TraceInfo("sdk取流ip:%s 端口:%d,线路号:%d",
		CGlobalParam::GetInstance()->m_SdkServerIp.c_str(),
		CGlobalParam::GetInstance()->m_nSdkServerPort,
		CGlobalParam::GetInstance()->m_nSdkServerLine);

	// 		g_logger.TraceInfo("rtmpIp:%s 端口:%d",
	// 			CGlobalParam::GetInstance()->m_RtmpServerIp.c_str(),
	// 			CGlobalParam::GetInstance()->m_nRtmpkServerPort);
	httpServer->run();
}
boost::shared_ptr<server> httpServer;
#endif

//定义全局函数变量
void Init();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void LogEvent(LPCSTR pszFormat, ...);
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);
CHAR szServiceName[] = ("wmp-pl");
BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
DWORD dwThreadID;


//*********************************************************
//Functiopn:			Init
//Description:			初始化
//Calls:				main
//Called By:				
//Table Accessed:				
//Table Updated:				
//Input:				
//Output:				
//Return:				
//Others:				
//History:				
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void Init()
{
    hServiceStatus = NULL;
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = SERVICE_STOPPED;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
}

void StartDeviceServer()
{
	std::string sSdkServerIp;
	int sSdkServerPort;
	std::string sUserName;
	std::string sPassWord;
	int nServerLine;

	std::string sHttpUrl;

	//xhkj
	std::string spagip;
	int npagPort;

	if(!initCrash())
	{
		printf("initCrash flase 程序将关闭");
		Sleep(10000);
		exit(0);
		return ;
	}

	CGlobalClass::GetInstance()->GetDevManager()->StartUpateDeviceInfo();

	#if 0
	GetPrivateProfileString("ZyrhOpenService","httpurl","",buf,2048,path.c_str());
	sHttpUrl = buf;

	GetPrivateProfileString("client","Sdkclient","",buf,2048,path.c_str());
	std::string str,str1,str2;
	str = buf;

	CdevSdkParam sCdevSdkParam;
	for (int i = 1;i<=5;i++)
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
		}
		str = str2;
	}

	GetPrivateProfileString("Gateway","Setpag","",buf,2048,path.c_str());
	str = buf;

	strseparate((char*)str.c_str(),str1,str2,"_");
	spagip = str1;
	npagPort = atoi(str2.c_str());

	unsigned int DevCount = GetPrivateProfileInt("Dev","Devcount",0,path.c_str());
	vector<DeviceServer *> 	vcDeviceServer;
	int 					commServerPort;
	int 					rtspServerPort;
	std::string 			deviceUserName;
	std::string 			deviceSecret;
	for (int i = 0;i<DevCount;i++)
	{
		//#设备ID=原设备ID_原设备取流线路_原设备通道数_新设备ID_域
		char sBufDevId[1024];
		sprintf(sBufDevId,"DevId_%d",i+1);
		GetPrivateProfileString("Dev",sBufDevId,"",buf,2048,path.c_str());
		str = buf;

		for (int j = 1;j<=7;j++)
		{
			strseparate((char*)str.c_str(),str1,str2,"_");
			switch(j)
			{
			case 1:
				sCdevSdkParam.m_sDevId = str1;
				break;
			case 2:
				sCdevSdkParam.m_nDevLine = atoi(str1.c_str());
				break;
			case 3:
				sCdevSdkParam.m_nnchannel = atoi(str1.c_str());
				break;
			case 4:
				commServerPort = atoi(str1.c_str());
				break;
			case 5:
				rtspServerPort = atoi(str1.c_str());
				break;
			case 6:
				deviceUserName = str1;
				break;
			case 7:
				deviceSecret = str1;
				break;
			default:
				break;
			}
			str = str2;

		}
		DeviceServer *stDeviceServer = new DeviceServer(commServerPort,rtspServerPort,deviceUserName,deviceSecret);
		stDeviceServer->setCdevSdkParam(sCdevSdkParam);
		stDeviceServer->start();
		vcDeviceServer.push_back(stDeviceServer); 	
		Sleep(100);
	}
	#endif
	while(1)
	{
		Sleep(10000);
		//printf("deviceServer is run \n");
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	StartDeviceServer();
	return 0;
}
//*********************************************************
//Functiopn:			ServiceMain
//Description:			服务主函数，这在里进行控制对服务控制的注册
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void WINAPI ServiceMain()
{
    // Register the control request handler
    status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	//注册服务控制
    hServiceStatus = RegisterServiceCtrlHandler((LPCSTR)szServiceName, ServiceStrl);
    if (hServiceStatus == NULL)
    {
        LogEvent(("Handler not installed"));
        return;
    }
    SetServiceStatus(hServiceStatus, &status);

    status.dwWin32ExitCode = S_OK;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
	status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hServiceStatus, &status);

	//模拟服务的运行，10后自动退出。应用时将主要任务放于此即可
	StartDeviceServer();

    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatus, &status);
    LogEvent(("Service stopped"));
}

//*********************************************************
//Functiopn:			ServiceStrl
//Description:			服务控制主函数，这里实现对服务的控制，
//						当在服务管理器上停止或其它操作时，将会运行此处代码
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:				dwOpcode：控制服务的状态
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void WINAPI ServiceStrl(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
		status.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hServiceStatus, &status);
        PostThreadMessage(dwThreadID, WM_CLOSE, 0, 0);
		//StopServer();
		exit(0);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
		exit(0);
        break;
    default:
        LogEvent(("Bad service request"));
    }
}
//*********************************************************
//Functiopn:			IsInstalled
//Description:			判断服务是否已经被安装
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL IsInstalled()
{
    BOOL bResult = FALSE;

	//打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
		//打开服务
        SC_HANDLE hService = ::OpenService(hSCM, (LPCSTR)szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

//*********************************************************
//Functiopn:			Install
//Description:			安装服务函数
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL Install()
{
    if (IsInstalled())
	{
		MessageBox(NULL, (" create service OK"), szServiceName, MB_OK);
        return TRUE;
	}

	//打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        MessageBox(NULL, ("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }

    // Get the executable file path
    CHAR szFilePath[MAX_PATH];
    ::GetModuleFileNameA(NULL, szFilePath, MAX_PATH);

	//创建服务
    SC_HANDLE hService = ::CreateService(
        hSCM, szServiceName, szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, (""), NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, ("Couldn't create service"), szServiceName, MB_OK);
        return FALSE;
	}
	MessageBox(NULL, (" create service OK"), szServiceName, MB_OK);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

//*********************************************************
//Functiopn:			Uninstall
//Description:			删除服务函数
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL Uninstall()
{
    if (!IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        //MessageBox(NULL, ("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        //MessageBox(NULL, ("Couldn't open service"), szServiceName, MB_OK);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	//删除服务
    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
        return TRUE;

    LogEvent(("Service could not be deleted"));
    return FALSE;
}

//*********************************************************
//Functiopn:			LogEvent
//Description:			记录服务事件
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void LogEvent(LPCSTR pFormat, ...)
{
    CHAR    chMsg[256];
    HANDLE  hEventSource;
    LPSTR  lpszStrings[1];
    va_list pArg;

    va_start(pArg, pFormat);
    _vstprintf(chMsg, pFormat, pArg);
    va_end(pArg);

    lpszStrings[0] = chMsg;
	hEventSource = RegisterEventSource(NULL, (LPCSTR)szServiceName);
	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
}


int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR     lpCmdLine,
					 int       nCmdShow)
{

	StartDeviceServer();
	return 0;
}


