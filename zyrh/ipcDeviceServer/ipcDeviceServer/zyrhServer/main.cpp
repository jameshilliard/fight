// alarm.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <xshell/xshell.hpp>
#include "stdafx.h"
#include "DevSDK/DevManager.h"

int main(int argc, char* argv[])
{
	Xshell<CDevManager> sh("wmp-plserver","WMPƽ̨����������.");
	int r = sh.Run(argc,argv);
	return r;
}
