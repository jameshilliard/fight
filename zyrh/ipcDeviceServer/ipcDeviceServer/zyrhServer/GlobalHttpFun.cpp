#include "GlobalHttpFun.h"
#include <GlobalClass.h>
#include <map>
#include <Windows.h>
std::string GetConnectType(std::string strFileNmae)
{

	static std::map<std::string,std::string> mapHttpConntentType;
	if (mapHttpConntentType.size() == 0)
	{
		mapHttpConntentType["ts"] = "video/mp2t";
		mapHttpConntentType["jpg"] = "image/jpeg";
		mapHttpConntentType["jpeg"] = "image/jpeg";
		mapHttpConntentType["m3u8"] = "audio/x-mpegurl";
		mapHttpConntentType["/live/CmdGet.m3u8"] = "audio/x-mpegurl";
		mapHttpConntentType["xml"] = "text/xml";
		mapHttpConntentType["mp3"] = "audio/mpeg";
		mapHttpConntentType["zip"] = "application/zip";
		mapHttpConntentType["mp4"] = "video/mp4";
		mapHttpConntentType["flv"] = "video/x-flv";
		mapHttpConntentType["html"] = "text/html";
		mapHttpConntentType["css"] = "text/css";
		mapHttpConntentType["gif"] = "image/gif";
		mapHttpConntentType["jpeg"] = "image/jpeg";
		mapHttpConntentType["jpg"] = "image/jpeg";
		mapHttpConntentType["js"] = "application/javascript";
		mapHttpConntentType["atom"] = "application/atom+xml";
		mapHttpConntentType["rss"] = "application/rss+xml";
	}
	char* strtype = (char*)strstr(strFileNmae.c_str(),".");
	std::string strRet = "text/html; charset=utf-8";
	if (strtype != NULL)
	{
		std::map<std::string,std::string>::iterator  it  = mapHttpConntentType.find((char*)strtype+1);
		if (it != mapHttpConntentType.end())
		{
			strRet = it->second;
		}
	}
	else
	{
		std::map<std::string,std::string>::iterator  it  = mapHttpConntentType.find(strFileNmae.c_str());
		if (it != mapHttpConntentType.end())
		{
			strRet = it->second;
		}

	}
	return strRet;
}
std::string GetCodeString(int httpCode)
{
	static std::map<int,std::string> mapHttpCode;
	if (mapHttpCode.size() == 0)
	{
		mapHttpCode[200] = "OK";
		mapHttpCode[201] = "Created";
		mapHttpCode[202] = "Accepted";
		mapHttpCode[203] = "Non-Authoritative Information";
		mapHttpCode[204] = "No Content";
		mapHttpCode[205] = "Reset Content";
		mapHttpCode[206] = "Partial Content";

		mapHttpCode[300] = "Multiple Choices";
		mapHttpCode[301] = "Moved Permanently";
		mapHttpCode[302] = "Found";
		mapHttpCode[303] = "See Other";
		mapHttpCode[304] = "Not Modified";
		mapHttpCode[305] = "Use Proxy";
		mapHttpCode[307] = "Temporary Redirect";

		mapHttpCode[400] = "Bad Request";
		mapHttpCode[401] = "Unauthorized";
		mapHttpCode[402] = "Payment Required";
		mapHttpCode[403] = "Forbidden";
		mapHttpCode[404] = "Not Found";
		mapHttpCode[405] = "ethod Not Allowed";
		mapHttpCode[406] = "Not Acceptable";
		mapHttpCode[405] = "ethod Not Allowed";
		mapHttpCode[406] = "Not Acceptable";
		mapHttpCode[407] = "Proxy Authentication Required";
		mapHttpCode[408] = "Request Time-out";

		mapHttpCode[409] = "Conflict";
		mapHttpCode[410] = "Gone";
		mapHttpCode[411] = "Length Required";
		mapHttpCode[412] = "Precondition Failed";
		mapHttpCode[413] = "Request Entity Too Large";
		mapHttpCode[414] = "Request-URI Too Large";

		mapHttpCode[415] = "Unsupported Media Type";
		mapHttpCode[416] = "Requested range not satisfiable";
		mapHttpCode[417] = "Expectation Failed";


		mapHttpCode[500] = "Internal Server Error";
		mapHttpCode[501] = "Not Implemented";
		mapHttpCode[502] = "Bad Gateway";
		mapHttpCode[503] = "Service Unavailable";
		mapHttpCode[504] = "Gateway Time-out";
		mapHttpCode[505] = "HTTP Version not supported";
	}
	std::string strRet;
	std::map<int,std::string>::iterator  it  = mapHttpCode.find(httpCode);
	if (it != mapHttpCode.end())
	{
		strRet = it->second;
	}
	return strRet;

}

std::string &   replace_all(std::string &   str,const   std::string &   old_value,const   std::string &   new_value)     
{     
	while(true)   {     
		string::size_type   pos(0);     
		if(   (pos=str.find(old_value))!=string::npos   )     
			str.replace(pos,old_value.length(),new_value);     
		else   break;     
	}     
	return   str;     
}     

std::string &   replace_all_distinct(std::string &   str,const   std::string &   old_value,const   std::string &   new_value)     
{     
	for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {     
		if(   (pos=str.find(old_value,pos))!=string::npos   )     
			str.replace(pos,old_value.length(),new_value);     
		else   break;     
	}     
	return   str;     
}    

std::string g_strtok( char * string,const char * control)
{
	char strbuf[2048];
	unsigned int nstrLen = strlen(string);
	std::string str;
	const char* pEnd = strstr(string,control);
	if (pEnd != NULL)
	{
		int nControlLen = pEnd - string;
		memcpy(strbuf,string,nControlLen);
		*(strbuf + (pEnd - string) ) = '\0';
		str =  strbuf;
	}
	return str;
}
void httpParamToMap(const std::string&   strHttp,std::map<std::string,std::string>& sParam)
{
	std::string str;
	str = strHttp;

	while (1)
	{
		std::string str1,str2,str3,str4;
		strseparate((char*)str.c_str(),str1,str2,"&");

		strseparate((char*)str1.c_str(),str3,str4,"=");
		sParam[str3] = str4;
		str = str2;
		if (str == "")
		{
			break;
		}
	}

}
void strseparate(char* src,std::string& sfrist,std::string& ssecond,const char* chSeparate)
{
	std::string str = src;
	char* sret = strtok((char*)str.c_str(),chSeparate);
	if (sret)
	{
		sfrist = sret;
		if (strcmp(sret,src)!=0)
		{
			ssecond = (src + strlen(sret) + strlen(chSeparate));
		}
		
	}
}
std::string GBKToUTF8(const std::string& strGBK)  
{  
	string strOutUTF8 = "";  
	WCHAR * str1;  
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);  
	str1 = new WCHAR[n];  
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);  
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);  
	char * str2 = new char[n];  
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);  
	strOutUTF8 = str2;  
	delete[]str1;  
	str1 = NULL;  
	delete[]str2;  
	str2 = NULL;  
	return strOutUTF8;  
}  
std::string UTF8ToGBK(const std::string& strUTF8)  
{  
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);  
	unsigned short * wszGBK = new unsigned short[len + 1];  
	memset(wszGBK, 0, len * 2 + 2);  
	MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);  

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);  
	char *szGBK = new char[len + 1];  
	memset(szGBK, 0, len + 1);  
	WideCharToMultiByte(CP_ACP,0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL, NULL);  
	//strUTF8 = szGBK;  
	std::string strTemp(szGBK);  
	delete[]szGBK;  
	delete[]wszGBK;  
	return strTemp;  
}  