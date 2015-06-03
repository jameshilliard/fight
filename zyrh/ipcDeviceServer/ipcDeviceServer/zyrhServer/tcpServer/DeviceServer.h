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
#define POCO_NO_UNWINDOWS
#include <iostream>
#include "../devsdk/CdevSdkParam.h"
#include "./DeviceCommFuncs.h"

#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"

using Poco::Timespan;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::Net::TCPServerConnection;
using Poco::Net::TCPServerConnectionFactory;
using Poco::Net::TCPServer;
using Poco::Net::TCPServerParams;
using Poco::Net::SocketAddress;


#define Poco_NO_UNWINDOWS

/*net client debug define*/
#define NET_INFO(x)  printf x
#define NET_ERR(x)   printf x
#define NET_FATAL(x) printf x

class CdevSdk;
class CThread;
class DeviceServerConnection: public TCPServerConnection
{
public:
	DeviceServerConnection(const StreamSocket& s,IpcDeviceParams *ipcDeviceParams,CdevSdk * nCdevSdk);
	STATUS reloveOnePacket(StreamSocket  &connfd,char *recvbuff,UINT32 bufferSize,void *param,UINT32 &mNetCmd);
	void run();
public:
	IpcDeviceParams *m_IpcDeviceParams;
	CdevSdk * m_nCdevSdk;
};

class DeviceServerConnectionFactory: public TCPServerConnectionFactory
{
public:
	DeviceServerConnectionFactory(IpcDeviceParams * ipcDeviceParams,CdevSdk * nCdevSdk);
	TCPServerConnection* createConnection(const StreamSocket& socket);
public:
	IpcDeviceParams *m_IpcDeviceParams;
	CdevSdk * m_nCdevSdk;
};


class DeviceServer
{
public:
	DeviceServer();
	~DeviceServer();
	void setCdevSdkParam(CdevSdkParam sCdevSdkParam,CdevSdk *mCdevSdk);
	void start();
	void stop();
	
public:
	void runCommServerActivity();
	void StartCommServerThread();

public:
	CThread					*m_commServerThread;
	bool					m_commServerStart;
	CdevSdk * m_nCdevSdk;
	IpcDeviceParams 	   *m_ipcDeviceParams;
};
#endif
