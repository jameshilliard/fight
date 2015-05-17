#include "GlobalFun.h"
#include <GlobalClass.h>
void RealeaseSendBuf(SendBufPtr sendBuf)
{
	CGlobalClass::GetInstance()->RealeaseSendBuf(sendBuf);
}
int CreatDir(char *pDir)  
{  
	int i = 0;  
	int iRet;  
	int iLen;  
	char pszDir[256];  

	if(NULL == pDir)  
	{  
		return 0;  
	}  

	strcpy(pszDir,pDir);  
	iLen = strlen(pszDir);  

	// 创建中间目录  
	for (i = 0;i < iLen;i ++)  
	{  
		if (pszDir[i] == '\\' || pszDir[i] == '/')  
		{   
			pszDir[i] = '\0';  

			//如果不存在,创建  
			iRet = ACCESS(pszDir,0);  
			if (iRet != 0)  
			{  
				iRet = MKDIR(pszDir);  
				if (iRet != 0)  
				{  
					return -1;  
				}   
			}  
			//支持linux,将所有\换成/  
			pszDir[i] = '/';  
		}   
	}  

	iRet = MKDIR(pszDir);   
	return iRet;  
}  