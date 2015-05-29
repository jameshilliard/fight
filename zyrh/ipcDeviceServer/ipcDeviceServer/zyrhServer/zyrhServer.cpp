// RtmpServer.cpp : �������̨Ӧ�ó������ڵ㡣
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

//����ȫ�ֺ�������
void Init();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void LogEvent(LPCSTR pszFormat, ...);
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);
CHAR szServiceName[] = ("wmp-plServer");
BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
DWORD dwThreadID;

#define SLEEP_TIME 5000
#define LOGFILE "G:\\memstatus.txt"


//*********************************************************
//Functiopn:			Init
//Description:			��ʼ��
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
	char buf[256];

	GetModuleFileNameA(NULL, buf, MAX_PATH);
	(_tcsrchr(buf,'\\'))[1] = 0;
	SetCurrentDirectory(buf);

	if(!initCrash())
	{
		printf("initCrash flase ���򽫹ر�");
		Sleep(10000);
		exit(0);
		return ;
	}
	WMP_Init();
	CGlobalClass::GetInstance()->GetDevManager()->StartUpateDeviceInfo();
	while(1)
	{
		Sleep(10000);
		//printf("deviceServer is run \n");
	}
}

//*********************************************************
//Functiopn:			ServiceMain
//Description:			��������������������п��ƶԷ�����Ƶ�ע��
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

	//ע��������
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

	//ģ���������У�10���Զ��˳���Ӧ��ʱ����Ҫ������ڴ˼���
	StartDeviceServer();

    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatus, &status);
    LogEvent(("Service stopped"));
}

//*********************************************************
//Functiopn:			ServiceStrl
//Description:			�������������������ʵ�ֶԷ���Ŀ��ƣ�
//						���ڷ����������ֹͣ����������ʱ���������д˴�����
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:				dwOpcode�����Ʒ����״̬
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
//Description:			�жϷ����Ƿ��Ѿ�����װ
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

	//�򿪷�����ƹ�����
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
		//�򿪷���
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
//Description:			��װ������
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
		//zssMessageBox(NULL, (" create service OK"), szServiceName, MB_OK);
        return TRUE;
	}

	//�򿪷�����ƹ�����
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        MessageBox(NULL, ("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }
    // Get the executable file path
    CHAR szFilePath[MAX_PATH];
    ::GetModuleFileNameA(NULL, szFilePath, MAX_PATH);

	//��������
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
//Description:			ɾ��������
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

	//ɾ������
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
//Description:			��¼�����¼�
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

	Init();
	dwThreadID = ::GetCurrentThreadId(); 
	SERVICE_TABLE_ENTRY st[] =
	{
		{ (LPSTR)szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};
	Install();
	if (stricmp(lpCmdLine, "-i") == 0)
	{
		MessageBox(NULL, ("Couldn't open service manager -i"), szServiceName, MB_OK);
		Install();
	}
	else if (stricmp(lpCmdLine, "-u") == 0)
	{
		Uninstall();
	}
	else
	{
		if (!::StartServiceCtrlDispatcher(st))
		{
			LogEvent(("Register Service Main Function Error!"));
		}
	}
	return 0;
}

