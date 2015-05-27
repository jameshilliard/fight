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
#include "DeviceServer.h"
#include "devsdk/DevSdk.h"

DeviceServerConnection::DeviceServerConnection(const StreamSocket& s,IpcDeviceParams *ipcDeviceParams,CdevSdk * nCdevSdk): 
	TCPServerConnection(s)
{
	m_IpcDeviceParams=ipcDeviceParams;
	m_nCdevSdk=nCdevSdk;

}

int concovtPtzData(NET_PTZ_CTRL_DATA netPTZCtrlData,ptzControl *mptzControl)
{
	if(mptzControl==NULL)
		return -1;
	if(netPTZCtrlData.command<100)
	{
		switch(netPTZCtrlData.command)
		{
		case GOTO_PRESET:
			mptzControl->ptz_cmd= WMP_PRESET_GOTO;
			break;	
		case SET_PRESET:
			mptzControl->ptz_cmd= WMP_PRESET_SET;
			break;
		case CLE_PRESET:
			mptzControl->ptz_cmd=  WMP_PRESET_DEL;
			break;	
		case TILT_UP:
			mptzControl->ptz_cmd= WMP_PTZ_UP;
			break;
		case TILT_DOWN:
			mptzControl->ptz_cmd= WMP_PTZ_DOWN;
			break;
		case PAN_LEFT:
			mptzControl->ptz_cmd= WMP_PTZ_LEFT;
			break;
		case PAN_RIGHT:
			mptzControl->ptz_cmd= WMP_PTZ_RIGHT;
			break;
		case UP_LEFT:
			mptzControl->ptz_cmd= WMP_PTZ_LEFT_UP;
			break;
		case UP_RIGHT:
			mptzControl->ptz_cmd=  WMP_PTZ_UP_RIGHT;
			break;
		case DOWN_LEFT:
			mptzControl->ptz_cmd= WMP_PTZ_DOWN_LEFT;
			break;
		case DOWN_RIGHT:
			mptzControl->ptz_cmd= WMP_PTZ_RIGHT_DOWN;
			break;
		case AUTO_PAN:
			mptzControl->ptz_cmd= WMP_PTZ_AUTO;
			break;	
		case ZOOM_IN:
			mptzControl->ptz_cmd=  WMP_PTZ_ZOOMIN;
			break;	
		case ZOOM_OUT:
			mptzControl->ptz_cmd=  WMP_PTZ_ZOOMOUT;
			break;			
		case FOCUS_IN:
			mptzControl->ptz_cmd=  WMP_PTZ_FOCUS_NEAR;
			break;	
		case FOCUS_OUT:
			mptzControl->ptz_cmd=  WMP_PTZ_FOCUS_FAR;
			break;	
		case IRIS_ENLARGE:
			mptzControl->ptz_cmd=  WMP_PTZ_DIAPHRAGM_ENLARGE;
			break;	
		case IRIS_SHRINK:
			mptzControl->ptz_cmd=  WMP_PTZ_DIAPHRAGM_SHRINK;
			break;	
		case  WIPER_PWRON:
			mptzControl->ptz_cmd=  WMP_PTZ_WIPER;
			break;
		case  LIGHT_PWRON:
			mptzControl->ptz_cmd=  WMP_PTZ_LIGHT;
			break;	
		case  HEATER_PWRON:
			mptzControl->ptz_cmd=  WMP_PTZ_HEAT;
			break;	
		default:
			break;
		}
		mptzControl->action=WMP_PTZ_ACTION_START;
	}
	else
	{
		mptzControl->action=WMP_PTZ_ACTION_STOP;
	}	
	mptzControl->param=netPTZCtrlData.presetNo;
	return 0;
}
	
STATUS DeviceServerConnection::reloveOnePacket(StreamSocket  &connfd,char *recvbuff,UINT32 bufferSize,void *param)
{
	NETCMD_HEADER netCmdHeader;
	UINT32 cmdLength, leftLen;
	INT32  retVal=-1;
	NET_PTZ_CTRL_DATA netPTZCtrlData;
	std::string ptzString="control ptz with speed";
	struct sockaddr_in  clientSockAddr, *pClientSockAddr;
	memcpy((char *)&netCmdHeader, recvbuff, sizeof(NETCMD_HEADER));
	netCmdHeader.netCmd = ntohl(netCmdHeader.netCmd);
	IpcDeviceParams *ptrIpcDeviceParams=(IpcDeviceParams *)param;
	NET_INFO(("server Port:%d rtspServer port:%d netCmdHeader.netCmd = 0x%0x \n",ptrIpcDeviceParams->m_commServerPort,ptrIpcDeviceParams->m_rtspServerPort,netCmdHeader.netCmd));
	switch(netCmdHeader.netCmd)
	{
	case NETCMD_LOGIN:           			/*user login*/			
		NET_INFO(("NETCMD_LOGIN.\n"));
		retVal = netClientLogin(connfd, recvbuff, pClientSockAddr,m_IpcDeviceParams->m_userName.c_str(),m_IpcDeviceParams->m_secret.c_str());
		break;
	case NETCMD_RELOGIN:          			/*user relogin*/
		NET_INFO(("NETCMD_RELOGIN.\n"));
		//retVal = netClientReLogin(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_LOGOUT:	          		/*user logout*/	
		NET_INFO(("NETCMD_LOGOUT.\n"));
		retVal = netClientLogout(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_USEREXCHANGE:     		/*user exchange*/
		NET_INFO(("NETCMD_USEREXCHANGE, time = 0x%x.\n", (int)time(NULL)));
		retVal = netClientUserExchange(connfd, recvbuff, pClientSockAddr);
		break;
	/*====================================================================
	*            				 		parameter config
	* ====================================================================*/	
	case NETCMD_GET_DEVICECFG:  		/*get device config param*/	
		NET_INFO(("NETCMD_GET_DEVICECFG.\n"));
		retVal = netClientGetDeviceCfg(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_GET_DEVICECFG_V40:			/*get device config param*/ 
		NET_INFO(("NETCMD_GET_DEVICECFG_V40.\n"));
		retVal = netClientGetDeviceCfgV40(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_GET_IPCORECFG_V31:        /*get IPC core config param, supports domain name*/
		NET_INFO(("NETCMD_GET_IPCORECFG_V31.\n"));
		//retVal = netClientGetIpCoreCfgV31(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_GET_NETCFG_V30:	    	/*get network config param*/	
		NET_INFO(("NETCMD_GET_NETCFG_V30.\n"));
		retVal = netClientGetNetCfgV30(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_GET_PREVIEWCFG_V30:    	/*get preview config param*/
		NET_INFO(("NETCMD_GET_PREVIEWCFG_V30.\n"));
		//retVal = netClientGetPrevCfgV30(connfd, recvbuff, pClientSockAddr, CHANNEL_MAIN);
		break;
	case NETCMD_GET_NETCFG:	    		/*get network config param*/	
		NET_INFO(("NETCMD_GET_NETCFG.\n"));
		retVal = netClientGetNetCfg(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_GET_RTSPPORT:
		NET_INFO(("NETCMD_GET_RTSPOLDCFG.\n"));
		retVal = netClientGetRtspPort(connfd, recvbuff, pClientSockAddr,param);
		break;
	case NETCMD_GET_COMPRESSCFG_V30:   /*get compress config param*/
		NET_INFO(("NETCMD_GET_COMPRESSCFG_V30.\n"));
		retVal = netClientGetCompressCfgV30(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_GET_PICCFG_V30:	  	/*get PIC V30 config param*/	
		NET_INFO(("NETCMD_GET_PICCFG_V30.\n"));
		retVal = netClientGetPicCfgV30(connfd, recvbuff, pClientSockAddr);
		break;
	case 0x11000:
		NET_INFO(("NETCMD_GET_11000.\n"));
		retVal = netClientReloveCmd11000(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_GET_ALARMINCFG_V30:    	/*get alarm in config param*/
		NET_INFO(("NETCMD_GET_ALARMINCFG_V30.\n"));
		retVal = netClientGetAlarmInCfgV30(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_RESTORE_CFG:				/*restore config*/	
		NET_INFO(("NETCMD_RESTORE_CFG.\n"));
		retVal = netClientRestoreCfg(connfd, recvbuff, pClientSockAddr);
		break;
	case NETCMD_PTZ:						/*PTZ ctrl not with speed*/
		NET_INFO(("NETCMD_PTZ.\n"));
		retVal = netClientPTZControl(connfd, recvbuff, pClientSockAddr,netPTZCtrlData);
		concovtPtzData(netPTZCtrlData,&m_IpcDeviceParams->m_ptzControl);
		if(m_nCdevSdk!=NULL)
			retVal = m_nCdevSdk->CmdPtzControl(m_IpcDeviceParams->m_sDevId,m_IpcDeviceParams->m_nnchannel,m_IpcDeviceParams->m_ptzControl.ptz_cmd,m_IpcDeviceParams->m_ptzControl.action,m_IpcDeviceParams->m_ptzControl.param,ptzString);
		break;
	case DVR_PTZWITHSPEED:          			/*PTZ ctrl with speed*/
		NET_INFO(("DVR_PTZWITHSPEED.\n"));
		retVal = netClientPTZControlWithSpeed(connfd, recvbuff, pClientSockAddr,netPTZCtrlData);
		concovtPtzData(netPTZCtrlData,&m_IpcDeviceParams->m_ptzControl);
		if(m_nCdevSdk!=NULL)
			retVal = m_nCdevSdk->CmdPtzControl(m_IpcDeviceParams->m_sDevId,m_IpcDeviceParams->m_nnchannel,m_IpcDeviceParams->m_ptzControl.ptz_cmd,m_IpcDeviceParams->m_ptzControl.action,m_IpcDeviceParams->m_ptzControl.param,ptzString);
		break;
	case NETCMD_TRANSPTZ:
		NET_INFO(("NETCMD_TRANSPTZ.\n"));
		break;
	case NETCMD_GETPTZCONTROL:
		NET_INFO(("NETCMD_GETPTZCONTROL.\n"));
		break;
	case NETCMD_GET_SHOWSTRING_V30:  		/*get OSD strings V30*/
		NET_INFO(("NETCMD_GET_SHOWSTRING_V30.\n"));
		retVal = netClientGetOSDCfgV30(connfd, recvbuff, pClientSockAddr);
		break;
	case 0x113406:
	case 0x113402:
	case 0x113400:
		NET_INFO(("netClientReloveCmd1134.\n"));
		retVal = netClientReloveCmd1134(connfd, recvbuff, pClientSockAddr,netCmdHeader.netCmd);
		break;
	case NETCMD_SET_TIMECFG:       		/*set time config param*/
		NET_INFO(("NETCMD_SET_TIMECFG.\n"));
		retVal = netClientSetDevTime(connfd, recvbuff, pClientSockAddr);
		break;
	default:
		NET_INFO(("no support cmd.\n"));
		char tempString[]={0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x1,0x00,0x00,0x00,0x1,0x00,0x00,0x00,0x0};
		retVal=connfd.sendBytes((char *)&tempString, sizeof(tempString),0);
		break;
	}		
	return retVal;
}

void DeviceServerConnection::run()
{
	int  cmdLength=0;
	char buffer[BUFFER_SIZE];
	Timespan mTimespan(30,0);
	socket().setReceiveTimeout(mTimespan);
	NET_INFO(("tcpServer:%d new connect\n",m_IpcDeviceParams->m_commServerPort));
	while(1)
	{
		int iRet =socket().receiveBytes(&cmdLength, sizeof(cmdLength));
		if(iRet==sizeof(cmdLength))
		{
			cmdLength=ntohl(cmdLength);
			if(cmdLength>20*1024)
			{
				NET_INFO(("tcpServer:%d receive data error %d\n",m_IpcDeviceParams->m_commServerPort,cmdLength));
				socket().shutdown();
				break;
			}
			iRet = socket().receiveBytes(buffer+sizeof(cmdLength),cmdLength-4);
			if(iRet==(cmdLength-4))
			{
				memcpy(buffer,&cmdLength,sizeof(cmdLength));
				iRet = reloveOnePacket(socket(),buffer,cmdLength,m_IpcDeviceParams);
				NET_INFO(("tcpServer:%d reloveOnePacket is %d\n",m_IpcDeviceParams->m_commServerPort,iRet));
			}
			else
			{
				NET_INFO(("tcpServer:%d receive data no enough is %d\n",m_IpcDeviceParams->m_commServerPort,iRet));
				socket().shutdown();
				break;
			}
		}
		else if(iRet<=0)
		{
			//NET_INFO(("tcpServer:%d iRet %d breakout\n",m_IpcDeviceParams->m_commServerPort,iRet));
			socket().shutdown();
			break;
		}
		Sleep(100);
	}
}
	
DeviceServerConnectionFactory::DeviceServerConnectionFactory(IpcDeviceParams *ipcDeviceParams,CdevSdk * nCdevSdk)
{
	m_IpcDeviceParams=ipcDeviceParams;
	m_nCdevSdk=nCdevSdk;
}

TCPServerConnection* DeviceServerConnectionFactory::createConnection(const StreamSocket& socket)
{
	return new DeviceServerConnection(socket,m_IpcDeviceParams,m_nCdevSdk);
}

DeviceServer::DeviceServer()
{
	m_ipcDeviceParams=new IpcDeviceParams();
	m_commServerThread = new CThread();
}

DeviceServer::~DeviceServer()
{
	free(m_ipcDeviceParams);
	free(m_commServerThread);
}

void DeviceServer::setCdevSdkParam(CdevSdkParam sCdevSdkParam,CdevSdk *mCdevSdk)
{
	m_nCdevSdk=mCdevSdk;
	m_ipcDeviceParams->m_commServerPort=sCdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort;
	m_ipcDeviceParams->m_rtspServerPort=sCdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort;
	m_ipcDeviceParams->m_userName=sCdevSdkParam.m_CdevChannelDeviceParam.m_sPlatDevName;
	m_ipcDeviceParams->m_secret=sCdevSdkParam.m_CdevChannelDeviceParam.m_sPlatDevPwd;
	m_ipcDeviceParams->m_sDevId=sCdevSdkParam.m_sDevId;
	m_ipcDeviceParams->m_nnchannel=sCdevSdkParam.m_CdevChannelDeviceParam.m_nChannelNo;

}

void DeviceServer::start()
{
	m_commServerStart=true;
	StartCommServerThread();
}

void DeviceServer::stop()
{
	m_commServerStart=false;
	m_commServerThread->StopThread();
}

void DeviceServer::StartCommServerThread()
{
	m_commServerThread->StartThread(boost::bind(&DeviceServer::runCommServerActivity,this));
}

void DeviceServer::runCommServerActivity()
{
	g_logger.TraceInfo("commServer start:%d",m_ipcDeviceParams->m_commServerPort);

	DeviceServerConnectionFactory *ptrDeviceServerConnectionFactory=new DeviceServerConnectionFactory(m_ipcDeviceParams,m_nCdevSdk);
	TCPServer srv(ptrDeviceServerConnectionFactory,m_ipcDeviceParams->m_commServerPort);
	srv.start();
	while(m_commServerStart)
	{
		//printf("commServer %d running. totalConnections£º%d refuse :%d\n",m_commServerPort,srv.totalConnections(),srv.refusedConnections());
		//printf("commServer %d running. maxConcurrentConnections£º%d currentConnections :%d\n",m_commServerPort,srv.maxConcurrentConnections(),srv.currentConnections());
		//printf("commServer %d running. queuedConnections£º%d\n",m_commServerPort,srv.queuedConnections());
		Sleep(1000);
	}
	srv.stop();
}





