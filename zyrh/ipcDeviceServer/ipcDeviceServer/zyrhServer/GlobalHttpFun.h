#ifndef _GLOBAL_HTTP_FUN_H
#define _GLOBAL_HTTP_FUN_H
#include <string>
#define  GET "GET"
#define  POST "POST"
#define  HEAD "HEAD"
#include <map>

std::string GetConnectType(std::string strFileNmae);
std::string GetCodeString(int httpCode);
std::string g_strtok( char * string,const char * control);
std::string&   replace_all(std::string&   str,const   std::string&   old_value,const   std::string&   new_value)  ;
std::string&   replace_all_distinct(std::string&   str,const   std::string&   old_value,const   std::string&   new_value)  ;
void strseparate(char* src,std::string& sfrist,std::string& ssecond,const char* chSeparate);
void httpParamToMap(const std::string&   str,std::map<std::string,std::string>& sParam);
std::string GBKToUTF8(const std::string& strGBK) ;
std::string UTF8ToGBK(const std::string& strUTF8)  ;
#endif