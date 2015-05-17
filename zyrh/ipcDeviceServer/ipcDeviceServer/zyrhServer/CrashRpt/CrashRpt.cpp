#include "CrashRpt.h"
#include <iostream>
BOOL WINAPI CrashCallback(LPVOID /*lpvState*/)  
{    
	// ����TRUE, ��crashrpt���ɴ��󱨸�  
	return TRUE;  
}  
bool initCrash()
{
	CR_INSTALL_INFO info;    
	memset(&info, 0, sizeof(CR_INSTALL_INFO));    
	info.cb = sizeof(CR_INSTALL_INFO);      
	info.pszAppName = ("MyApp");    
	info.pszAppVersion = ("1.0.0");    
	info.pszEmailSubject = ("MyApp 1.0.0 Error Report");    
	info.pszEmailTo = ("myapp_support@hotmail.com");      
	info.pszUrl = ("http://myapp.com/tools/crashrpt.php"); 
	info.pszErrorReportSaveDir = TEXT("./crashrpt");  
	info.pfnCrashCallback = CrashCallback;     
	info.uPriorities[CR_HTTP] = 3;  // ����ʹ��HTTP�ķ�ʽ���ʹ��󱨸�  
	info.uPriorities[CR_SMTP] = 2;  // Ȼ��ʹ��SMTP�ķ�ʽ���ʹ��󱨸�    
	info.uPriorities[CR_SMAPI] = 1; //�����ʹ��SMAPI�ķ�ʽ���ʹ��󱨸�      
	// ���������ܹ�������쳣, ʹ��HTTP�����Ʊ���ķ�ʽ����  
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;  
	info.dwFlags |= CR_INST_HTTP_BINARY_ENCODING;   
	//info.dwFlags |= CR_INST_APP_RESTART;   
	info.dwFlags |= CR_INST_DONT_SEND_REPORT;     
	// ��˽����URL  
	info.pszPrivacyPolicyURL = ("http://myapp.com/privacypolicy.html");   

	int nResult = crInstall(&info);      
	if(nResult!=0)    
	{      
		TCHAR szErrorMsg[512] = ("");          
		crGetLastErrorMsg(szErrorMsg, 512);      
		printf(("%s\n"), szErrorMsg);      
		return false;  
	}   
	// ����������Ϣ�����󱨸��У��������Կ���Ϣ��Ϊʾ��  
	crAddProperty(("VideoCard"), ("nVidia GeForce 8600 GTS"));
	return true;

}