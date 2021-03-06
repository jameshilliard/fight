#pragma once 
#include <string>
#include <thread.h>
#include <ffmpegEncoder.h>
#include <aac/AACEncoder.h>
#include <publicHead.h>
#include "wmpclient.h"
#include "ts/M3U8List.h"
#include "g722/HCNetSDK.h"
#include "g711/g711.h"
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <bas/io_service_pool.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/detail/mutex.hpp>
#include "./CdevSdkParam.h"
#include "../tcpServer/DeviceServer.h"

typedef struct OneFramePacket_
{
	bool bKey;
	uint32_t bufsize;
	__int64 TimeStamp;
	std::string buffer;
}OneFramePacket;

class  CdevSdk:public  boost::enable_shared_from_this<CdevSdk>
{
public:
	CdevSdk();
	~CdevSdk();
	int StartDev(char* sServerIp,unsigned int nServerPort,unsigned int nServerLine,char* sdevId,char* password,unsigned int nDevLine,unsigned int nchannel,int nType);
	int StartDev(CdevSdkParam cdevSdkParam);
	void StopDev();
	bool ReStartDev();
	bool StartDev();
	bool InPutPsData(unsigned char* videoPsBuf,unsigned int  psBufsize,int nType );
	void handleVideo(uint8_t* vidoedata,uint32_t nsize,__int64 TimeStamp,bool bkey);
	void handleAudioAac(uint8_t* aacbuf,uint32_t bufsize,__int64 timeStamp,int nChannel,int nSoundRate);

	bool Ps_AnalyzeDataGetPacketEx();
	void StartVideoEncoderThread();
	void StartAudioEncoderThread();
	std::string CreateM3u8File();
	void OnTime(const boost::system::error_code& e);
	int CmdPtzControl(std::string sdevid,unsigned int nchannel,int ptz_type,int ptz_cmd,int action,int param,std::string& smsg );

	//zss++
	bool GetVideoData(unsigned char *ptData,unsigned int &frameSize,unsigned int dataMaxSize,unsigned int &fNumTruncatedBytes,unsigned int &lelfPackNums);
	//bool addDeviceSource(std::vector<std::string > *vDeviceSource);
	//bool removeDeviceSource(std::vector<std::string > *vDeviceSource);
	void CdevSdk::runRtspServerActivity();
	int  CdevSdk::startRtspServer();
	void CdevSdk::stopRtspServerThread();
	void CdevSdk::StartRtspServerThread();
	void RtspOnTime(const boost::system::error_code& e);
	void StopPlay();
	void GetKeyFrame();
	//zss--
private:
	void ResetParam();
	WMP_HANDLE m_wmp_handle;
	
	
public:
	char* m_h264Buf;
	int m_stream_handle;
	unsigned int m_nLength;


	std::string m_sServerIp;
	unsigned int m_nServerPort;
	unsigned int m_nServerLine;

	std::string m_spassword;
	unsigned int m_nDevLine;
	unsigned int m_nnchannel;
	std::string m_sDevId;

	LONG m_nSkdPlayPort;

	//音频参数
	int m_nSampleRate;//采样率
	int m_nNumChannels;//通道数

	int m_nVideoType;

	void*  m_pG722;
	int m_nAnalyzeHandle;
	void *m_nAnalyzeHandlePtr;
	int m_nSystem_Format;
	int m_firstFramePrefix;
	boost::asio::detail::mutex mutex_Lock;
	boost::asio::detail::mutex mutex_HandleVideo;
	bool m_firstKey;
	std::vector<OneFramePacket > m_deviceSource;
private:
	int m_nTimeNow;

	CThread m_pThread;
	CThread m_pThreadAudio;

	void handleEncoder();
	void handleEncoderAudio();
public:
	unsigned int m_nIndex;

	CMyList<boost::shared_ptr<buffer>> m_VideoBuflist;
	CMyList<boost::shared_ptr<buffer>> m_AudioBuflist;
	CFFmpegEncoder* m_pffmpegEncoder;

	CMyList<boost::shared_ptr<buffer>> m_SendBuflist;
	CAacEncoder* m_pAacEncoder;
	CM3u8List m_pM3u8List;

	typedef boost::shared_ptr<boost::asio::deadline_timer> timer_ptr;
	#if 0
	timer_ptr m_io_timer_Ptr;
	timer_ptr m_rtsp_timer_Ptr;
	boost::asio::io_service* m_pIoService;
	boost::asio::io_service* m_pRtspService;
	#endif
	int nTimeTest;
	int m_nType;
	std::string m_srtmpurl;
	bool m_bStop;
	int m_nCheckDelTimeOut;

	//zss++
	CThread m_rtspServerThread;
	bool	m_rtspServerStart;
	bool    m_restartSDKFlag;
	unsigned int m_nLastPlatDevPort;
	unsigned int m_lastFrameSize;
	unsigned int m_lastTrute;
	CdevSdkParam m_CdevSdkParam;
	CdevSdkParam m_LastCdevSdkParam;
	char	m_watchVariable;
	char	m_rtspEndFlag;
	DeviceServer m_DeviceServer;
	DWORD  m_rtspTime;
	OneFramePacket m_keyFramePacket;
	//zss--
};
