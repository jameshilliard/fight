#ifndef _GLOBAL_FUN_H
#define _GLOBAL_FUN_H
#include "GlobalHttpFun.h"
#include <string>
#include <stdlib.h>  
#include <string.h> 
using namespace std;
#include <bas/Sendbuffer.hpp>
#ifdef _WIN32  
#include <direct.h>  
#include <io.h>  
#elif _LINUX  
#include <stdarg.h>  
#include <sys/stat.h>  
#endif  

#ifdef _WIN32  
#define ACCESS _access  
#define MKDIR(a) _mkdir((a))  
#elif _LINUX  
#define ACCESS access  
#define MKDIR(a) mkdir((a),0755)  
#endif  

extern "C" void  RealeaseSendBuf(SendBufPtr);
int			CreatDir(char *pDir);

#endif