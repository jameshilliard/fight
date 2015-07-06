#ifndef _INCnetCommandFuncs_h
#define _INCnetCommandFuncs_h
#include "./DeviceCommProtocol.h"
#include "./netUtil.h"
#include "Poco/Net/StreamSocket.h"
using Poco::Net::StreamSocket;

enum
{
	BUFFER_SIZE = 1024*128
};

struct ptzControl
{
	unsigned int 	nchannel;
	int 			ptz_cmd;
	int 			ptz_type;
	int 			action;
	int 			param;
	std::string 	smsg;
};

typedef struct {
	int 					m_commServerPort;
	int 					m_rtspServerPort;
	int 					m_beatEnable;
	int						m_isOnline;
	std::string 			m_userName;
	std::string 			m_secret;    
	std::string 			m_sDevId;
	unsigned int 			m_nnchannel;
	std::string 			m_sLocalIpaddr;
	struct ptzControl		m_ptzControl;
} IpcDeviceParams;

UINT32 checkByteSum(char *pBuf, int len);
STATUS challenge_login(StreamSocket  &connfd, NET_LOGIN_REQ *reqdata, int *userid,struct sockaddr_in *pClientSockAddr,const char *localUser,const char *localSecret);
STATUS netClientLogin(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr,const char *localUser,const char *localSecret);
STATUS netClientLogout(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientReLogin(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientUserExchange(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientGetNetCfgV30(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientGetRtspPort(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr,void *param);
STATUS netClientGetDeviceCfgV40(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientGetDeviceCfg(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientGetCompressCfgV30(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientGetNetCfg(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientReloveCmd11000(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientSetDevTime(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientGetPicCfgV30(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientGetAlarmInCfgV30(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientRestoreCfg(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientReloveCmd1134(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr,UINT32 cmdType);
STATUS netClientPTZControl(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr,NET_PTZ_CTRL_DATA &netPTZCtrlData);
STATUS netClientPTZControlWithSpeed(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr,NET_PTZ_CTRL_DATA &netPTZCtrlData);
STATUS netClientGetOSDCfgV30(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);
STATUS netClientGetWorkStatusV30(StreamSocket  &connfd, char *recvbuff, struct sockaddr_in *pClientSockAddr);

#endif
