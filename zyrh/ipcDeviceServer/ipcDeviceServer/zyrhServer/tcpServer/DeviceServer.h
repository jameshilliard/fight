//
// DeviceServer.cpp
//
// $Id: //poco/1.4/Net/samples/DeviceServer/src/DeviceServer.cpp#1 $
//
// This sample demonstrates the TCPServer and ServerSocket classes.
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//
#ifndef _INCDeviceServer_h
#define _INCDeviceServer_h
#include <iostream>
#include "../devsdk/H264FrameDeviceSource.h"
#include "../devsdk/SDKServerData.h"
#include "./DeviceCommFuncs.h"


#define Poco_NO_UNWINDOWS

#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Activity.h"
#include "Poco/Thread.h"
#include "Poco/Mutex.h"

using Poco::Thread;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::Net::TCPServerConnection;
using Poco::Net::TCPServerConnectionFactory;
using Poco::Net::TCPServer;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Mutex;



/*net client debug define*/
#define NET_INFO(x)  printf x
#define NET_ERR(x)   printf x
#define NET_FATAL(x) printf x



class DeviceServerConnection: public TCPServerConnection
{
public:
	DeviceServerConnection(const StreamSocket& s,IpcDeviceParams *ipcDeviceParams,H264FrameDeviceSource * nH264FrameDeviceSource);
	STATUS reloveOnePacket(StreamSocket  &connfd,char *recvbuff,UINT32 bufferSize,void *param);
	void run();
public:
	IpcDeviceParams *m_IpcDeviceParams;
	H264FrameDeviceSource * m_nH264FrameDeviceSource;
};

class DeviceServerConnectionFactory: public TCPServerConnectionFactory
{
public:
	DeviceServerConnectionFactory(IpcDeviceParams * ipcDeviceParams,H264FrameDeviceSource * nH264FrameDeviceSource);
	TCPServerConnection* createConnection(const StreamSocket& socket);
public:
	IpcDeviceParams *m_IpcDeviceParams;
	H264FrameDeviceSource * m_nH264FrameDeviceSource;
};

class DeviceServer
{
public:
	DeviceServer();
	DeviceServer(int port,int rtspPort,std::string userName,std::string secret);
	~DeviceServer();
	void setSdkServerData(SDKServerData sSDKserverData);
	void start();
	void stop();
	
public:
	void runCommServerActivity();
	void runRtspServerActivity();
	void StartCommServerThread();
	void StartRtspServerThread();

public:
	int startRtspServer(int rtpServerPort);

	CThread m_commServerThread;
	CThread m_rtspServerThread;
	
	bool					m_commServerStart;
	bool					m_rtspServerStart;
	H264FrameDeviceSource * m_nH264FrameDeviceSource;
	IpcDeviceParams 	  *	m_ipcDeviceParams;
	SDKServerData 			m_sdkServerData;
	int 					m_commServerPort;
	int 					m_rtspServerPort;
	std::string 			m_userName;
	std::string 			m_secret;
};
#endif
