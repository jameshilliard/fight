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

class CdevSdkParam
{
public:

	CdevSdkParam()
	{
		m_sSdkServerIp="";
		m_nSdkServerPort=0;
		m_spassword="";
		m_sUserName="";
		m_nServerLine=0;

		m_nDevLine=0;
		m_nnchannel=0;
		m_sDevId="";
	}

	CdevSdkParam(std::string sSdkServerIp,unsigned int nSdkServerPort,std::string sPassword,
		std::string sUserName,unsigned int nServerLine,unsigned int nDevLine,
		unsigned int nChannel,std::string sDevId)
	{
		m_sSdkServerIp=sSdkServerIp;
		m_nSdkServerPort=nSdkServerPort;
		m_spassword=sPassword;
		m_sUserName=sUserName;
		m_nServerLine=nServerLine;

		m_nDevLine=nDevLine;
		m_nnchannel=nChannel;
		m_sDevId=sDevId;
	};
public:	
	//sdk 信息
	std::string 	m_sSdkServerIp;
	unsigned int 	m_nSdkServerPort;
	std::string	 	m_spassword;
	std::string 	m_sUserName;
	unsigned int 	m_nServerLine;

	unsigned int 	m_nDevLine;
	unsigned int 	m_nnchannel;
	std::string 	m_sDevId;

};

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
	int CmdPtzControl(std::string sdevid,unsigned int nchannel,int ptz_cmd,int action,int param,std::string& smsg );

	//zss++
	bool GetVideoData(std::vector<std::string > *vDeviceSource,unsigned char *ptData,unsigned int &frameSize,unsigned int dataMaxSize,unsigned int &curVideoIndex);
	bool addDeviceSource(std::vector<std::string > *vDeviceSource);
	bool removeDeviceSource(std::vector<std::string > *vDeviceSource);
	//zss--
private:
	void ResetParam();
	WMP_HANDLE m_wmp_handle;
	void StopPlay();
	
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
	int m_nSystem_Format;
	boost::asio::detail::mutex mutex_Lock;
	boost::asio::detail::mutex mutex_HandleVideo;
	
	std::vector<std::vector<std::string > *> m_deviceSource;
private:
	int m_nTimeNow;

	CThread m_pThread;
	CThread m_pThreadAudio;

	//zss++
	CdevSdkParam m_CdevSdkParam;
	//zss--
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
	timer_ptr m_io_timer_Ptr;
	boost::asio::io_service* m_pIoService;
	int nTimeTest;
	int m_nType;
	std::string m_srtmpurl;
	bool m_bStop;
	int m_nCheckDelTimeOut;
};
