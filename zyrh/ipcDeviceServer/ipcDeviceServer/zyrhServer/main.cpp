// alarm.cpp : 定义控制台应用程序的入口点。
//

#include <xshell/xshell.hpp>
#include "stdafx.h"
#include "DevSDK/DevManager.h"

int main(int argc, char* argv[])
{
	Xshell<CDevManager> sh("wmp-plserver","WMP平台级联服务器.");
	int r = sh.Run(argc,argv);
	return r;
}
