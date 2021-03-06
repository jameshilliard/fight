#include "stdafx.h"
#include "DevSdk.h"
#include <iostream>
#include "WindowsPlayM4.h"
#include <GlobalClass.h>
#include <GlobalParam.h>
#include "DevManager.h"


#define		VERSION_4_0_0		1
#define		VERSION_3_0_0		0
#define		ANALYZEDATETYPE		VERSION_3_0_0
//#define		DEBUG_FILE			1

#if	ANALYZEDATETYPE==VERSION_3_0_0
#include "AnalyzeDataInterface.h"
#elif	ANALYZEDATETYPE==VERSION_4_0_0
#include "AnalyzeDataDefine.h"
#include "AnalyzeDataNewInterface.h"
#endif

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "../rtspServer/H264LiveVideoServerMediaSubssion.hh"
#include "../rtspServer/H264FramedLiveSource.hh"

#define MYSYSTEM_MPEG2_PS   0x02	// PS封装



static void CALLBACK DecCBFun(long nPort,char * pBuf,long nSize,
					   FRAME_INFO * pFrameInfo, 
					   long nReserved1,long nReserved2)
{

	//---------------------------------------
	// 获取解码后音频数据
	boost::shared_ptr<CdevSdk> DevPtr = CGlobalClass::GetInstance()->GetDevManager()->GetDev(nReserved1);
	if (!DevPtr)
	{
		return;
	}
	//---------------------------------------
	// 获取解码后视频数据
	if ( pFrameInfo->nType == T_YV12 ) 
	{   
		boost::shared_ptr<buffer> sBufferPtr(new buffer(nSize));
		append_data((char*)sBufferPtr->data,pBuf,nSize);
		sBufferPtr->TimeStamp = pFrameInfo->nStamp;
		sBufferPtr->d_cur = nSize;

		static int nTime = time(NULL);
		if (time(NULL) > nTime + 5 )
		{
			nTime = time(NULL);
		}
		if (!DevPtr->m_pffmpegEncoder)
		{
			DevPtr->m_pffmpegEncoder = new CFFmpegEncoder(pFrameInfo->nWidth,pFrameInfo->nHeight,pFrameInfo->nFrameRate);
			DevPtr->StartVideoEncoderThread();
		}
		DevPtr->m_VideoBuflist.pushBack(sBufferPtr);
	}
}
//长度为14个字节
struct Ps_packae_header
{
	unsigned int   unpack_start_code:32;//  32 bslbf 
	unsigned int  nVlaude01:2;//'01'  2 bslbf 
	unsigned int  system_clock_reference_base32_30:3;//  3 bslbf 
	unsigned int  marker_bit0:1;// 1 bslbf 
	unsigned int system_clock_reference_base29_15:15;//  15 bslbf  包含五个和六个字节
	unsigned int  marker_bit1:1;// 1 bslbf 
	unsigned int  system_clock_reference_base14_0:15;//15 bslbf 
	unsigned int  marker_bit2:1;//  1  bslbf 
	unsigned int  system_clock_reference_extension:9;//  9  uimsbf 
	unsigned int  marker_bit3:1;//  1 bslbf 
	unsigned int  program_mux_rate:22;//22  uimsbf 
	unsigned int  marker_bit4:1;//1 bslbf 
	unsigned int  marker_bit5:1;//1 bslbf 

	unsigned int  reserved:5;//  5  bslbf 
	unsigned int  pack_stuffing_length:3;// 3 uimsbf

	char stuffing_byte[7]; 

	__int64  nTimeStamp;


	Ps_packae_header()
	{
		//ZeroMemory(this,sizeof(Ps_packae_header));
	}

	char* SetValue( char* svalue)
	{
		char* pstart = svalue;
		unsigned short nTmp = 0;
		unpack_start_code = get_byte32(svalue);
		svalue += 4;
		//第五个字节
		unsigned char nbyte = *svalue++;
		nVlaude01 = (nbyte&0xC0)>>6;
		system_clock_reference_base32_30 = (nbyte&0x38)>>3;
		marker_bit0 = (nbyte&04)>>2;
		system_clock_reference_base29_15 = (nbyte&0x03)<<13; //占用2个字节

		//第六个字节
		nbyte = *svalue++;
		system_clock_reference_base29_15 = system_clock_reference_base29_15|(nbyte<<5);

		//第七个字节
		nbyte = *svalue++;
		system_clock_reference_base29_15 = system_clock_reference_base29_15|(nbyte&0xF8)>>3;//占用五个字节
		marker_bit1 = (nbyte&0x04)>>2;
		system_clock_reference_base14_0 = (nbyte&0x03)<<13; //占用两个字节

		//第八个字节
		nbyte = *svalue++;
		system_clock_reference_base14_0 = system_clock_reference_base14_0|(nbyte)<<5;

		//第九个字节
		nbyte = *svalue++;
		system_clock_reference_base14_0 = system_clock_reference_base14_0|(nbyte&0xF8)>>3; //占用五个字节
		marker_bit2 = (nbyte&0x04)>>2;
		system_clock_reference_extension = (nbyte&0x10)<<7; //占用两个字节


		//第十个字节
		nbyte = *svalue++;
		system_clock_reference_extension = system_clock_reference_extension|(nbyte&0xFE)>>1; //占用七个字节
		marker_bit3 = (nbyte&0x01);

		//第十一个字节
		nbyte = *svalue++;
		program_mux_rate = nbyte<<14; //共22位


		//第十二个字节
		nbyte = *svalue++;
		program_mux_rate = program_mux_rate|nbyte<<6; 

		//第十三个字节
		nbyte = *svalue++;
		program_mux_rate = program_mux_rate|(nbyte&0xFC)>>2;  ////占用六个字节
		marker_bit4 = (nbyte&0x02)>>1;
		marker_bit5 = (nbyte&0x01);

		//第十四个字节
		nbyte = *svalue++;
		reserved = (nbyte&0xF8)>>3;
		pack_stuffing_length = (nbyte&0x07);

		// 		if (pack_stuffing_length>0)
		// 		{
		// 			memcpy(stuffing_byte,svalue,pack_stuffing_length);
		// 		}

		svalue += pack_stuffing_length;
		__int64 system_clock_reference_base29_15_tmp  = system_clock_reference_base29_15;
		__int64 system_clock_reference_base32_30_tmp = system_clock_reference_base32_30;
		nTimeStamp = (system_clock_reference_base29_15_tmp<<15)|system_clock_reference_base14_0|(system_clock_reference_base32_30_tmp<<30);

		return svalue;
	}

};
struct pes_packae_header
{
	unsigned int  unpack_start_code:32;//  32 bslbf 
	unsigned int  PES_packet_length:16;// 包长 

	unsigned int  nVlaude01:2;//  2 bslbf 
	unsigned int  PES_scrambling_control :2;// 1 bslbf 
	unsigned int  PES_priority :1;//  1 bslbf  
	unsigned int  data_alignment_indicator :1;// 1 bslbf   
	unsigned int  copyright:1;//1 bslbf 
	unsigned int  original_or_copy:1;//1 bslbf 

	unsigned int  PTS_DTS_flags:2;//  2  bslbf 
	unsigned int  ESCR_flag:1;//  1  bslbf 
	unsigned int  ES_rate_flag:1;//  1 bslbf 
	unsigned int  DSM_trick_mode_flag:1;//1  bslbf 
	unsigned int  additional_copy_info_flag:1;//1 bslbf 
	unsigned int  PES_CRC_flag:1;//1 bslbf 
	unsigned int  PES_extension_flag:1;//1 bslbf

	unsigned int  PES_header_data_length:8;// 16 bslbf

	//假设PTS_DTS_flags == 0x02 
	unsigned int  nValue0010:4;//4 bslbf 
	unsigned int  nPTS32_30:3; //3  bslbf 
	unsigned int  marker_bit0:1;//1  bslbf 
	unsigned int  PTS29_15:15;//15 bslbf 
	unsigned int  marker_bit1:1;// 1  bslbf 
	unsigned int  PTS14_0:15;//15  bslbf 
	unsigned int  marker_bit2:1;// 1 bslbf
	__int64  nTimeStamp;

	char stuffing_byte[0xf];

	char *packagedata;
	unsigned int packagedatalen;



	pes_packae_header()
	{
		//ZeroMemory(this,sizeof(Ps_packae_header));
		nTimeStamp = 0;
		packagedata = NULL;
	}
	~pes_packae_header()
	{
		if(packagedata)
		{
			delete[] packagedata;
			packagedata = NULL;
		}
	}

	char* SetValue( char* svalue)
	{
		char* pstart = svalue;

		unpack_start_code = get_byte32(svalue);
		svalue += 4;
		//第五、六字节
		PES_packet_length = get_byte16(svalue);
		svalue += 2;
		//第七个字节
		unsigned char nbyte = *svalue++;
		nVlaude01 = (nbyte&0xC0)>>6;
		PES_scrambling_control = (nbyte&0x30)>>4;
		PES_priority = (nbyte&0x08)>>3;
		data_alignment_indicator = (nbyte&0x04)>>2; 
		copyright = (nbyte&0x02)>>1;
		original_or_copy = (nbyte&0x01);

		//第八个字节
		nbyte = *svalue++;
		PTS_DTS_flags = (nbyte&0xC0)>>6; ;//  2  bslbf 
		ESCR_flag = (nbyte&0x20)>>5;//  1  bslbf 
		ES_rate_flag = (nbyte&0x10)>>4;;//  1 bslbf 
		DSM_trick_mode_flag = (nbyte&0x08)>>3;//1  bslbf 
		additional_copy_info_flag = (nbyte&0x40)>>2;//1 bslbf 
		PES_CRC_flag = (nbyte&0x20)>>1;//1 bslbf 
		PES_extension_flag = (nbyte&0x01);//1 bslbf

		//第九个字节
		nbyte = *svalue++;
		PES_header_data_length = nbyte;
		unsigned int header_data_length = PES_header_data_length;

		if (PTS_DTS_flags == 0x02)
		{
			unsigned short n16Tmp;
			//第十个字节
			nbyte = *svalue++;
			nValue0010 = (nbyte&0xF0)>>4;//4 bslbf 
			nPTS32_30 = (nbyte&0x0E)>>1; //3  bslbf 
			marker_bit0 = (nbyte&0x01);//1  bslbf 

			//第十一个字节
			nbyte = *svalue++;
			n16Tmp  = nbyte<<7;

			//第十二个字节
			nbyte = *svalue++;
			PTS29_15 = n16Tmp|(nbyte&0xFE)>>1;
			marker_bit1 = (nbyte&0x01);

			//第十三个字节
			nbyte = *svalue++;
			n16Tmp = nbyte<<7;

			//第十四个字节
			nbyte = *svalue++;
			PTS14_0 = n16Tmp|(nbyte&0xFE)>>1;
			marker_bit2 = (nbyte&0x01);

			__int64 nPTS32_30_tmp = nPTS32_30;
			__int64 PTS29_15_tmp = PTS29_15;
			nTimeStamp = (nPTS32_30_tmp<<30)|(PTS29_15_tmp<<15)|PTS14_0;
			header_data_length -= 5;
		}
		else if (PTS_DTS_flags == 0x03)
		{
		}
		else if (PTS_DTS_flags == 0x00)
		{
		}
		memcpy(stuffing_byte,svalue,header_data_length);
		svalue += header_data_length; 
		if (svalue - pstart <  PES_packet_length + 6 )
		{
			if (packagedata != NULL)
			{
				delete[] packagedata;
				packagedata = NULL;

			}
			packagedatalen = PES_packet_length + 6 - (svalue - pstart) ;
			packagedata = new char[packagedatalen];
			memcpy(packagedata,svalue,packagedatalen);
			svalue += packagedatalen; 
		}
		return svalue;
	}

};

struct ps_system_header  { 
	unsigned int    system_header_start_code:32;//  32 bslbf 
	unsigned int 	header_length:16;//  16 uimsbf 
	unsigned int 	marker_bit0:1;//  1 bslbf 
	unsigned int 	rate_bound:22;//  22 uimsbf 
	unsigned int 	marker_bit1:1;//  1 bslbf 
	unsigned int 	audio_bound:6;//  6 uimsbf 
	unsigned int 	fixed_flag:1;//  1 bslbf 
	unsigned int 	CSPS_flag:1;//  1 bslbf 
	unsigned int 	system_audio_lock_flag:1;//1 bslbf  system_audio_lock_flag
	unsigned int 	system_video_lock_flag:1;//1 bslbf  
	unsigned int 	rker_bit:1;//  1 bslbf  ma
	unsigned int 	video_bound:5;//  5  uimsbf
	unsigned int 	packet_rate_restriction_flag:1;//  1  bslbf 
	unsigned int 	reserved_byte:7;//  7 bslbf 
	// 		while (nextbits () == '1'){ 
	// 			stream_id  8  uimsbf
	// 				'11'  2 bslbf 
	// 				P-STD_buffer_bound_scale  1  bslbf 
	// 				P-STD_buffer_size_bound  13  uimsbf 
	// 		} 
	void SetValue( char* svalue)
	{
		system_header_start_code = get_byte32(svalue);
		svalue += 4;
		//第五个字节 和第六个字节
		header_length = get_byte16(svalue);
		svalue += 2;

		//第七个字节
		unsigned char nbyte = *svalue++;
		marker_bit0 = (nbyte&0x80)>>7;
		rate_bound = ((nbyte&0x7F)>>1)<<15; //占用七位

		//第八个字节
		nbyte = *svalue++;
		rate_bound = rate_bound|(nbyte<<7);

		//第九个字节
		nbyte = *svalue++;
		rate_bound = rate_bound|(nbyte&0xFE)>>1; //占用七位
		marker_bit1 = nbyte&0x01;

		//第十个字节
		nbyte = *svalue++;
		audio_bound = audio_bound|(nbyte&0xFC)>>2; //占用七个字节
		fixed_flag = (nbyte&0x02)>>1;
		CSPS_flag = (nbyte&0x01);

		//第十一个字节
		nbyte = *svalue++;
		system_audio_lock_flag = (nbyte&0x80)>>7;
		system_video_lock_flag = (nbyte&0x40)>>6;
		rker_bit = (nbyte&0x20)>>5;
		video_bound = (nbyte&0x1F);

		//第十二个字节
		nbyte = *svalue++;
		packet_rate_restriction_flag = (nbyte&0x80)>>7;
		reserved_byte = (nbyte&0x7F)>>1;
	}
} ;

void CBF_OnStreamPlay(int h,int sflag,const char* data  ,unsigned int data_size,void* ctx)
{
	unsigned int nIndex = (unsigned int)ctx;
	CdevSdk * DevPtr = (CdevSdk *)ctx;
	if (!DevPtr)
	{
		return;
	}
	DevPtr->InPutPsData((unsigned char *)data,data_size,sflag);
		
}
CdevSdk::CdevSdk()
{
	m_nSkdPlayPort = -1;
	m_h264Buf = new char[512*1024];
	m_pffmpegEncoder = NULL;
	m_pAacEncoder = NULL;
	m_pG722 = NULL;
	m_nAnalyzeHandle = -1;
	m_nAnalyzeHandlePtr=NULL;
	m_nTimeNow = time(NULL);
	#if 0
	boost::asio::io_service& _IoService = CGlobalClass::GetInstance()->GetIoservice()->get_io_service();
	m_io_timer_Ptr.reset(new boost::asio::deadline_timer(_IoService) );
	m_pIoService = &_IoService;

	boost::asio::io_service& _RtspService = CGlobalClass::GetInstance()->GetIoservice()->get_io_service();
	m_rtsp_timer_Ptr.reset(new boost::asio::deadline_timer(_RtspService) );
	m_pRtspService = &_RtspService;

	#endif
	
	m_firstKey=true;
	m_restartSDKFlag=false;
	m_bStop = false;
	m_rtspTime=GetTickCount();
	//m_nServerLine=0;
	m_nLastPlatDevPort=0;
	m_nCheckDelTimeOut = 0;

	m_stream_handle=-1;
	m_wmp_handle=-1;
	//m_firstFramePrefix=-1;
	m_rtspEndFlag=0;
}

CdevSdk::~CdevSdk()
{
	delete[] m_h264Buf;
	#if 0
	if (m_io_timer_Ptr)
	{
		m_io_timer_Ptr.reset();
		 
	}
	if (m_rtsp_timer_Ptr)
	{
		m_rtsp_timer_Ptr.reset();

	}
	#endif
	stopRtspServerThread();
	m_DeviceServer.stop();
}
#if 0
void CdevSdk::OnTime(const boost::system::error_code& e)
{
// 	if ( e == boost::asio::error::operation_aborted)
// 	{
// 		return;
// 	}
	if(m_bStop)
	{
		return;
	}
	if (!m_io_timer_Ptr)
	{
		return;
	}
	if (time(NULL) - m_nTimeNow > 30)
	{
		g_logger.TraceInfo("取流超时，重新取流 %s, %d in ",m_sDevId.c_str(),m_nnchannel);
		ReStartDev();
		g_logger.TraceInfo("取流超时，重新取流 %s, %d out ",m_sDevId.c_str(),m_nnchannel);	
	}
	g_logger.TraceInfo("设备取流正常 %s:%d  ",m_sDevId.c_str(),m_nnchannel);	
	m_io_timer_Ptr->expires_from_now(boost::posix_time::seconds(10));
	m_io_timer_Ptr->async_wait(boost::bind(&CdevSdk::OnTime,shared_from_this(),boost::asio::placeholders::error));

}

void CdevSdk::RtspOnTime(const boost::system::error_code& e)
{
	// 	if ( e == boost::asio::error::operation_aborted)
	// 	{
	// 		return;
	// 	}
	if(m_bStop)
	{
		return;
	}
	if (!m_rtsp_timer_Ptr)
	{
		return;
	}
	//if(m_restartSDKFlag==true && (m_wmp_handle == -1))
	//{
		//g_logger.TraceInfo("sdk 控制端口:%d rtsp服务端口:%d ReStartDev",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort);
		//ReStartDev();
		//m_restartSDKFlag=false;
	//}
	if (m_watchVariable==1)
	{
		g_logger.TraceInfo("sdk 控制端口:%d rtsp服务端口:%d 重启rtsp服务器",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort);
		if(m_CdevSdkParam.m_isChannelEnable==1 && m_CdevSdkParam.m_isOnline==1)
			StartRtspServerThread();
	}
	m_rtsp_timer_Ptr->expires_from_now(boost::posix_time::seconds(5));
	m_rtsp_timer_Ptr->async_wait(boost::bind(&CdevSdk::RtspOnTime,shared_from_this(),boost::asio::placeholders::error));
}
#endif
int CdevSdk::StartDev(char* sServerIp,unsigned int nServerPort,unsigned int nServerLine,char* sdevId,char* password,unsigned int nDevLine,unsigned int nchannel,int nType)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	m_sServerIp = sServerIp;
	m_nServerPort = nServerPort;
	m_nServerLine = nServerLine;
	m_spassword = password;
	m_nDevLine = nDevLine;;
	m_nnchannel = nchannel;
	m_sDevId = sdevId;
	m_pM3u8List.m_sdveid = sdevId;
	m_wmp_handle = WMP_Create();
	if (m_wmp_handle == -1)
	{
		return -1;
	}
	int ret =  WMP_Login(m_wmp_handle,sServerIp,nServerPort, sdevId,password,nServerLine);
	if (ret == -1)
	{
		return -1;
	}
	ret = WMP_Play(m_wmp_handle,
		sdevId,//devid:设备ID
		nchannel,//channel:设备通道号
		WMP_STREAM_MAIN, //stream_type:WMP_STREAM_MAIN 1-主码流   WMP_STREAM_SUB 2-子码流 
		WMP_TRANS_TCP,//trans_mode:WMP_TRANS_TCP/WMP_TRANS_UDP  #define WMP_TRANS_TCP	1#define WMP_TRANS_UDP	2
		nDevLine,//dev_line:设备线路号
		CBF_OnStreamPlay, this,(int*)&m_stream_handle);
	g_logger.TraceInfo("sdk取流 设备ID:%s 设备通道号:%d,设备线路号:%d,m_wmp_handle:%d,取流返回:%d ",m_sDevId.c_str(),m_nnchannel,m_nDevLine,m_wmp_handle,ret);
	#if 0
	m_io_timer_Ptr->expires_from_now(boost::posix_time::seconds(5));
	m_io_timer_Ptr->async_wait(boost::bind(&CdevSdk::OnTime,shared_from_this(),boost::asio::placeholders::error));
	#endif
	return ret;
}

int CdevSdk::StartDev(CdevSdkParam cdevSdkParam)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	m_sServerIp = cdevSdkParam.m_sSdkServerIp;
	m_nServerPort = cdevSdkParam.m_nSdkServerPort;
	m_nServerLine = cdevSdkParam.m_nServerLine;
	m_spassword = cdevSdkParam.m_spassword;
	m_nDevLine = cdevSdkParam.m_nDevLine;
	m_nnchannel = cdevSdkParam.m_CdevChannelDeviceParam.m_nChannelNo;
	m_sDevId = cdevSdkParam.m_sDevId;
	m_pM3u8List.m_sdveid = cdevSdkParam.m_sDevId;
	if(cdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort==0)
		cdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort=m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort;
	m_CdevSdkParam=cdevSdkParam;
	m_DeviceServer.setCdevSdkParam(m_CdevSdkParam,this);
	//stopRtspServerThread();
	//Sleep(200);
	if (m_wmp_handle != -1)
	{
		StopPlay();
	}
	if(m_CdevSdkParam.m_isChannelEnable==1)
	{
		if(m_nLastPlatDevPort==0)
		{
			m_nLastPlatDevPort=m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort;
		}	
		else if(m_nLastPlatDevPort != m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort)
		{
			m_DeviceServer.stop();
			m_nLastPlatDevPort=m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort;
		}	
		m_DeviceServer.start();
		StartRtspServerThread();
		#if 0
		m_rtsp_timer_Ptr->expires_from_now(boost::posix_time::seconds(5));
		m_rtsp_timer_Ptr->async_wait(boost::bind(&CdevSdk::RtspOnTime,shared_from_this(),boost::asio::placeholders::error));
		#endif
	}
	return 0;
}


bool CdevSdk::StartDev()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if(m_wmp_handle != -1)
	{
		return false;
	}
	StopPlay();
	m_AudioBuflist.clear();
	m_VideoBuflist.clear();
	m_wmp_handle = WMP_Create();
	if (m_wmp_handle == -1)
	{
		return false;
	}
	int ret =  WMP_Login(m_wmp_handle,m_sServerIp.c_str(),m_nServerPort, m_sDevId.c_str(),m_spassword.c_str(),m_nServerLine);
	if (ret == -1)
	{
		return false;
	}
	ret = WMP_Play(m_wmp_handle,
		m_sDevId.c_str(),//devid:设备ID
		m_nnchannel,//channel:设备通道号
		WMP_STREAM_MAIN, //stream_type:WMP_STREAM_MAIN 1-主码流   WMP_STREAM_SUB 2-子码流 
		WMP_TRANS_UDP,//trans_mode:WMP_TRANS_TCP/WMP_TRANS_UDP  #define WMP_TRANS_TCP	1#define WMP_TRANS_UDP	2
		m_nDevLine,//dev_line:设备线路号
		CBF_OnStreamPlay, (void*)this,(int *)&m_stream_handle);
	g_logger.TraceInfo("sdk 控制端口:%d rtsp服务端口:%d ,sdk重新取流 设备ID:%s 设备通道号:%d,设备线路号:%d,m_wmp_handle:%d,取流返回:%d ",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort,m_sDevId.c_str(),m_nnchannel,m_nDevLine,m_wmp_handle,ret);
	#if 0
	m_io_timer_Ptr->expires_from_now(boost::posix_time::seconds(5));
	m_io_timer_Ptr->async_wait(boost::bind(&CdevSdk::OnTime,shared_from_this(),boost::asio::placeholders::error));
	#endif
	if (ret != 0)
	{
		return false;
	}
	return true;
}
bool CdevSdk::ReStartDev()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if(((GetTickCount()-m_rtspTime)<12000) && (m_wmp_handle != -1))
	{
		printf("nowtime is %d - m_rtspTime %d = %d",GetTickCount(),m_rtspTime,(GetTickCount()-m_rtspTime));
		return true;
	}	
	m_deviceSource.clear();
	GetKeyFrame();
	if(m_CdevSdkParam.m_isOnline==0 || m_CdevSdkParam.m_isChannelEnable==0)
	{
		if(m_CdevSdkParam.m_isOnline!=m_LastCdevSdkParam.m_isOnline || m_CdevSdkParam.m_isChannelEnable!=m_LastCdevSdkParam.m_isChannelEnable)
		{
			m_LastCdevSdkParam=m_CdevSdkParam;
			g_logger.TraceInfo("sdk 控制端口:%d rtsp服务端口:%d 重新取流 设备ID:%s 设备通道号:%d,设备线路号:%d,不在线 %d %d",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort,m_sDevId.c_str(),m_nnchannel,m_nDevLine,m_CdevSdkParam.m_isOnline,m_CdevSdkParam.m_isChannelEnable);
		}
		return false;
	}
	if (m_wmp_handle != -1)
	{
		StopPlay();
	}
	if(m_watchVariable==1)
	{
		StartRtspServerThread();
	}
	m_AudioBuflist.clear();
	m_VideoBuflist.clear();
	m_wmp_handle = WMP_Create();
	if (m_wmp_handle == -1)
	{
		return false;
	}
	int ret =  WMP_Login(m_wmp_handle,m_sServerIp.c_str(),m_nServerPort, m_sDevId.c_str(),m_spassword.c_str(),m_nServerLine);
	if (ret == -1)
	{
		return false;
	}
	unsigned int stream_type=0;
	unsigned int nConnectType=0;
	if(m_CdevSdkParam.m_CdevChannelDeviceParam.m_nStreamType==0)
		stream_type=1;
	else
		stream_type=2;
	if(m_CdevSdkParam.m_CdevChannelDeviceParam.m_nConnectType==0)
		nConnectType=2;
	else
		nConnectType=1;
	ret = WMP_Play(m_wmp_handle,
		m_sDevId.c_str(),//devid:设备ID
		m_nnchannel,//channel:设备通道号
		stream_type, //stream_type:WMP_STREAM_MAIN 1-主码流   WMP_STREAM_SUB 2-子码流 
		nConnectType,//trans_mode:WMP_TRANS_TCP/WMP_TRANS_UDP  #define WMP_TRANS_TCP	1#define WMP_TRANS_UDP	2
		m_nDevLine,//dev_line:设备线路号
		CBF_OnStreamPlay, this,(int *)&m_stream_handle);
	g_logger.TraceInfo("sdk 控制端口:%d rtsp服务端口:%d 重新取流 设备ID:%s 设备通道号:%d,主次码流:%d,传输类型:%d,设备线路号:%d,取流返回:%d "
	,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort,m_sDevId.c_str(),m_nnchannel,stream_type,nConnectType,m_nDevLine,ret);
		
	if (ret != 0)
	{
		return false;
	}	
	m_rtspTime=GetTickCount();
	return true;
}
void CdevSdk::ResetParam()
{
	m_wmp_handle = -1;

}
int CdevSdk::CmdPtzControl(std::string sdevid,unsigned int nchannel,int ptz_type,int ptz_cmd,int action,int param,std::string& smsg )
{
	int nSdkRet = -1;
	if (m_wmp_handle == -1)
	{
		smsg = "设备未登陆";
		return nSdkRet;
	}
	g_logger.TraceInfo("设备ID:%s 设备通道号:%d,云台命令:%d,云台操作:%d,云台参数:%d ",sdevid.c_str(),m_nnchannel,ptz_cmd,action,param);
	if(ptz_type != 1)
		nSdkRet = WMP_PtzControl(m_wmp_handle,sdevid.c_str(),nchannel,ptz_cmd,action,param);
	else
		nSdkRet = WMP_SetPreset(m_wmp_handle,sdevid.c_str(),nchannel,ptz_cmd,param,"test");
	smsg = "sdk ret";
	return nSdkRet;
}
void CdevSdk::StopDev()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	m_bStop = true;
	#if 0
	if (m_io_timer_Ptr)
	{
		m_io_timer_Ptr->cancel();
	}
	if (m_rtsp_timer_Ptr)
	{
		m_rtsp_timer_Ptr->cancel();
	}
	#endif
	if (m_wmp_handle != -1)
	{
		WMP_Stop(m_wmp_handle,m_stream_handle);
		WMP_Close(m_wmp_handle);
		m_wmp_handle = -1;
	}
	if (m_pffmpegEncoder)
	{
		m_pThread.StopThread();
		g_logger.TraceInfo("Video StopThread() %s,",m_sDevId.c_str());
		delete m_pffmpegEncoder;
		m_pffmpegEncoder = NULL;

	}
	m_pThreadAudio.StopThread();
	if (m_pAacEncoder)
	{
		g_logger.TraceInfo("Audio StopThread() %s,",m_sDevId.c_str());
		delete m_pAacEncoder;
		m_pAacEncoder = NULL;
	}
	if (m_nSkdPlayPort != -1)
	{

		PlayM4_Stop(m_nSkdPlayPort);

		//关闭流，回收源数据缓冲
		PlayM4_CloseStream(m_nSkdPlayPort);

		//释放播放库端口号
		PlayM4_FreePort(m_nSkdPlayPort);

		m_nSkdPlayPort = -1;

	}

	#if ANALYZEDATETYPE==VERSION_3_0_0
	if (m_nAnalyzeHandle != -1)
	{
		AnalyzeDataClose(m_nAnalyzeHandle);
		g_logger.TraceInfo("停止解包 m_nAnalyzeHandle %d devid:%s",m_nAnalyzeHandle,m_sDevId.c_str());
		m_nAnalyzeHandle = -1;
	}
	#else ANALYZEDATETYPE==VERSION_4_0_0
	if(m_nAnalyzeHandlePtr != NULL)
	{
		HIKANA_Destroy(m_nAnalyzeHandlePtr);
		g_logger.TraceInfo("停止解包 m_nAnalyzeHandle %d devid:%s",m_nAnalyzeHandlePtr,m_sDevId.c_str());
		m_nAnalyzeHandlePtr = NULL;	
                   
	}
	#endif
	
	if (m_pG722)
	{
		NET_DVR_ReleaseG722Decoder(m_pG722);
		m_pG722 = NULL;
	}

}
void CdevSdk::StopPlay()
{
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
		m_deviceSource.clear();	
	}
	if (m_wmp_handle != -1)
	{
		WMP_Stop(m_wmp_handle,m_stream_handle);
		WMP_Close(m_wmp_handle);
		m_wmp_handle = -1;
	}

	if (m_nSkdPlayPort != -1)
	{

		PlayM4_Stop(m_nSkdPlayPort);

		//关闭流，回收源数据缓冲
		PlayM4_CloseStream(m_nSkdPlayPort);

		//释放播放库端口号
		PlayM4_FreePort(m_nSkdPlayPort);

		m_nSkdPlayPort = -1;

	}

	#if ANALYZEDATETYPE==VERSION_3_0_0
	if (m_nAnalyzeHandle != -1)
	{
		AnalyzeDataClose(m_nAnalyzeHandle);
		g_logger.TraceInfo("sdk 控制端口:%d rtsp服务端口:%d 关闭设备 m_nAnalyzeHandle %d devid:%s",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort,m_nAnalyzeHandlePtr,m_sDevId.c_str());
		m_nAnalyzeHandle = -1;
	}
	#else ANALYZEDATETYPE==VERSION_4_0_0
	if (m_nAnalyzeHandlePtr != NULL)
	{
		HIKANA_Destroy(m_nAnalyzeHandlePtr);
		g_logger.TraceInfo("sdk 控制端口:%d rtsp服务端口:%d 关闭设备 m_nAnalyzeHandle %d devid:%s",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort,m_nAnalyzeHandlePtr,m_sDevId.c_str());
		m_nAnalyzeHandlePtr = NULL;
	}
	#endif
	
}
bool CdevSdk::GetVideoData(unsigned char *ptData,unsigned int &fFrameSize,unsigned int dataMaxSize,unsigned int &fNumTruncatedBytes,unsigned int &lelfPackNums)
{
	int frameSize=0;
	bool hasKeyFrame=false;
	fFrameSize=0;
	fNumTruncatedBytes=0;
	OneFramePacket videoBuffer;
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
		std::vector<OneFramePacket >::iterator it;
		if(m_firstKey==true && (!m_deviceSource.empty()))
		{
			int count=0;
			for(it=m_deviceSource.begin(); it!= m_deviceSource.end(); it++)
			{
				count++;
				//printf("packet %d size %d---\n",count,it->bufsize);
				if(it->bKey==true)
				{
					//printf("keytimeStamp is %d %d---\n",m_keyFramePacket.buffer.size(),m_keyFramePacket.TimeStamp);
					hasKeyFrame=true;
					m_firstKey=false;
					break;
				}
			}
			if(hasKeyFrame==true)
			{		
				//printf("delete 1 %d packet---\n",count-1);
				if(count!=1)
					m_deviceSource.erase(m_deviceSource.begin(),m_deviceSource.begin()+count-1);
			}
			else
			{
				//printf("delete 2 %d packet-m_keyFramePacket.buffer.size=%d--\n",m_deviceSource.size(),m_keyFramePacket.buffer.size());
				if(m_keyFramePacket.buffer.size()>0)
				{
					it=m_deviceSource.begin();
					int validKeyTime=it->TimeStamp-m_keyFramePacket.TimeStamp;
					if(validKeyTime<5000 && validKeyTime>-5000)
					{
						m_firstKey=false;
						m_deviceSource.insert(m_deviceSource.begin(),m_keyFramePacket);
					}
					else
						m_deviceSource.clear();
				}
				else
				{
					m_deviceSource.clear();
					m_rtspTime=GetTickCount();
				}
			}
		}
		if(!m_deviceSource.empty())
		{
			m_rtspTime=GetTickCount();
			it=m_deviceSource.begin();
			videoBuffer=*it;
			frameSize=videoBuffer.buffer.size();
			if(frameSize<=dataMaxSize)
			{
				m_deviceSource.erase(it);
			}
			lelfPackNums=m_deviceSource.size();
		}
	}
	if(frameSize>dataMaxSize)
	{
		memset(ptData,0,dataMaxSize);
		fFrameSize=dataMaxSize;
		fNumTruncatedBytes=frameSize-dataMaxSize;
		return false;
	}	
	else if(frameSize>0)
	{	
		if(videoBuffer.bKey==true)
			printf("tcp=%d,rtsp=%d,time=%d,leftPackNum=%d,thisPtr=0x%x,fFrameSize=%d,fMaxSize=%d,timeStamp=%ld\n",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort,GetTickCount(),lelfPackNums,this,frameSize,dataMaxSize,videoBuffer.TimeStamp);
		memcpy(ptData,videoBuffer.buffer.c_str(),frameSize);
		fFrameSize=frameSize;
		fNumTruncatedBytes=0;
		return true;
	}
	return false;
}

std::string CdevSdk::CreateM3u8File()
{
	return m_pM3u8List.CreateM3u8File();
}
bool CdevSdk::InPutPsData(unsigned char* videoPsBuf,unsigned int  psBufsize,int nType )
{
	if(nType == 1)
	{
		_HIK_MEDIAINFO_* p_HIK_MEDIAINFO_ = (_HIK_MEDIAINFO_*)videoPsBuf;
		m_nSystem_Format = p_HIK_MEDIAINFO_->system_format;
		m_nNumChannels = p_HIK_MEDIAINFO_->audio_channels;
		m_nSampleRate = p_HIK_MEDIAINFO_->audio_samplesrate;
		m_nVideoType = p_HIK_MEDIAINFO_->audio_format;
		if (m_nVideoType == AUDIO_G722_1)
		{
			m_pG722 = NET_DVR_InitG722Decoder(m_nSampleRate);
		}
		if (m_nSystem_Format == MYSYSTEM_MPEG2_PS)
		{
			//m_firstFramePrefix=0;
			#if ANALYZEDATETYPE==VERSION_3_0_0
			m_nAnalyzeHandle = AnalyzeDataGetSafeHandle();
			AnalyzeDataOpenStreamEx(m_nAnalyzeHandle, (PBYTE)videoPsBuf);	
			#else ANALYZEDATETYPE==VERSION_4_0_0
			m_nAnalyzeHandlePtr = HIKANA_CreateStreamEx(500*1024,(PBYTE)videoPsBuf);
			#endif
			g_logger.TraceInfo("sdk 控制端口:%d rtsp服务端口:%d 取流成功 m_nAnalyzeHandle %d devid:%s",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort,m_nAnalyzeHandlePtr,m_sDevId.c_str());
			#ifdef		DEBUG_FILE
			FILE * m_pFileStream;
			//char temp[16]={0};
			//memset(temp,0x55,sizeof(temp));
			char filePath[128]={0};
			sprintf(filePath,"h264//%d_%d_org.h264",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort);
			//以追加的方式打开文件流
			m_pFileStream = fopen(filePath, "ab+");
			//fwrite((uint8_t *)temp,sizeof(temp),1,m_pFileStream);
			//fwrite((uint8_t *)(&nType),sizeof(nType),1,m_pFileStream);
			fwrite((unsigned char *)videoPsBuf,psBufsize,1,m_pFileStream);
			fflush(m_pFileStream);
			fclose(m_pFileStream);		
			#endif	
		}
		else if(m_nSkdPlayPort == -1)
		{
			PlayM4_GetPort(&m_nSkdPlayPort);
			//设置流模式类型 
			PlayM4_SetStreamOpenMode(m_nSkdPlayPort,STREAME_REALTIME); 
			//打开流模式
			PlayM4_OpenStream(m_nSkdPlayPort,(PBYTE)videoPsBuf,psBufsize,1024 * 1024);

			PlayM4_SetDecCallBackExMend(m_nSkdPlayPort,DecCBFun,NULL,0,(long)m_nIndex);
			PlayM4_Play(m_nSkdPlayPort,0);
			PlayM4_PlaySound(m_nSkdPlayPort);
			
		}	
		
		if (m_nVideoType == AUDIO_G722_1)
		{
			m_pG722 = NET_DVR_InitG722Decoder(m_nSampleRate);
		}
		if (!m_pAacEncoder&&m_nVideoType != 0)
		{
			m_pAacEncoder = new CAacEncoder(m_nSampleRate,m_nNumChannels);
		}
		StartAudioEncoderThread();	
	}
	else
	{
		m_nTimeNow = time(NULL);
		if (m_nSystem_Format == MYSYSTEM_MPEG2_PS)
		{
			
			#ifdef		DEBUG_FILE
			FILE * m_pFileStream;
			//char temp[16]={0};
			//memset(temp,0x55,sizeof(temp));
			char filePath[128]={0};
			sprintf(filePath,"h264//%d_%d_org.h264",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort);
			//以追加的方式打开文件流
			m_pFileStream = fopen(filePath, "ab+");
			//fwrite((uint8_t *)temp,sizeof(temp),1,m_pFileStream);
			//fwrite((uint8_t *)(&nType),sizeof(nType),1,m_pFileStream);
			fwrite((unsigned char *)videoPsBuf,psBufsize,1,m_pFileStream);
			fflush(m_pFileStream);
			fclose(m_pFileStream);		
			#endif
			//if(psBufsize>4 && m_firstFramePrefix==0)
			//{
			//	if(videoPsBuf[0]==0x0 && videoPsBuf[1]==0x0 && videoPsBuf[2]==0x1 && videoPsBuf[3]==0xBA)
			//	{
			//		memcpy(&m_firstFramePrefix,videoPsBuf,sizeof(m_firstFramePrefix));
			//	}
			//	else
			//		return true;
			//}

			#if ANALYZEDATETYPE==VERSION_3_0_0
			AnalyzeDataInputData(m_nAnalyzeHandle, videoPsBuf, psBufsize);	
			#else ANALYZEDATETYPE==VERSION_4_0_0
			HIKANA_InputData(m_nAnalyzeHandlePtr, videoPsBuf, psBufsize);
			#endif
			
			while(1)
			{
				if(Ps_AnalyzeDataGetPacketEx())
				{
					continue;
				}
				break;
			}
		}
		else
		{
			bool bFlag = PlayM4_InputData(m_nSkdPlayPort,(PBYTE)videoPsBuf,psBufsize);
		}
	}
	return true;
}
void CdevSdk::StartVideoEncoderThread()
{
	m_pThread.StartThread(boost::bind(&CdevSdk::handleEncoder,this));
}
void CdevSdk::StartAudioEncoderThread()
{
	m_pThreadAudio.StartThread(boost::bind(&CdevSdk::handleEncoderAudio,this));
}
void CdevSdk::handleEncoder()
{
	boost::shared_ptr<buffer> videoBufinPtr;
	if (m_VideoBuflist.DelData(videoBufinPtr))
	{
		buffer outH264Buf;
		if (videoBufinPtr)
		{
			unsigned int nWidth = m_pffmpegEncoder->m_nWidth;
			unsigned int nHigth = m_pffmpegEncoder->m_nHegint;
			AVFrame *pframe = m_pffmpegEncoder->m_frame;
			int nSize = videoBufinPtr->d_cur;
			char* pBuf = (char*)videoBufinPtr->data;
			int nNum = nWidth*nHigth;
			memcpy(pframe->data[0],pBuf,nSize);
			memcpy(pframe->data[2],pBuf+nNum,nNum/4);
			memcpy(pframe->data[1],pBuf+nNum/4*5,nNum/4);
			bool bkey = false;
			m_pffmpegEncoder->Encode_frame(&outH264Buf,bkey);
			if (outH264Buf.d_cur>0)
			{
				m_nTimeNow = time(NULL);
				handleVideo(outH264Buf.data,outH264Buf.d_cur, videoBufinPtr->TimeStamp,bkey);
			}
		}
	}
	static int nTime = time(NULL);
	if (time(NULL) > nTime + 5 )
	{
		//g_logger.TraceInfo("handleEncoder video data");
		nTime = time(NULL);
	}
	if (m_VideoBuflist.GetSize() > 50)
	{
		g_logger.TraceInfo("%s m_VideoBuflist.clear  %d",m_sDevId.c_str(),m_VideoBuflist.GetSize());
		m_VideoBuflist.clear();
		m_AudioBuflist.clear();
	}

}
void CdevSdk::handleEncoderAudio()
{
	boost::shared_ptr<buffer> audioBufinPtr;
	boost::shared_ptr<buffer> aacBufinPtr(new buffer);
	if (m_AudioBuflist.DelData(audioBufinPtr)&&m_pAacEncoder)
	{
		m_pAacEncoder->EncEncodeAac((char*)audioBufinPtr->data,audioBufinPtr->d_cur,aacBufinPtr);
		if (aacBufinPtr->d_cur>7)
		{
			handleAudioAac(aacBufinPtr->data + 7,aacBufinPtr->d_cur - 7,audioBufinPtr->TimeStamp,m_nNumChannels,m_nSampleRate);
		}
	}
	boost::shared_ptr<buffer> videoBufinPtr;
	if (m_SendBuflist.DelData(videoBufinPtr))
	{
		handleVideo(videoBufinPtr->data,videoBufinPtr->d_cur,videoBufinPtr->TimeStamp,videoBufinPtr->bkey);
	}
}
void CdevSdk::handleAudioAac(uint8_t* aacbuf,uint32_t bufsize,__int64 timeStamp,int nChannel,int nSoundRate)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
	//m_pM3u8List.handleAudio(aacbuf,bufsize,timeStamp);
	//if(m_nType&0x01)
	{
	
	}
	
	

}
void CdevSdk::handleVideo(uint8_t* vidoebuf,uint32_t bufsize,__int64 TimeStamp,bool bkey)
{
	unsigned int size=0;
	//std::string temp((char *)vidoebuf,bufsize);
	OneFramePacket tempOneFramePacket;
	tempOneFramePacket.bKey=bkey;
	tempOneFramePacket.TimeStamp=TimeStamp;
	tempOneFramePacket.bufsize=bufsize;
	tempOneFramePacket.buffer.assign((const char *)vidoebuf,bufsize);
	{
		if(bkey==true)
		{
			printf("Get keyFrame size is %d\n",bufsize);
			m_keyFramePacket=tempOneFramePacket;
		}
		boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
		{
			size=m_deviceSource.size();
			if(size<200)
			{
				m_deviceSource.push_back(tempOneFramePacket);
			}
			else
			{
				g_logger.TraceInfo("tcp %d,rtsp %d,insert data size is %d stask %d\n",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort,bufsize,size);	
				m_deviceSource.clear();
			}
		}
	}
	if(((GetTickCount()-m_rtspTime)>6000) && (m_wmp_handle != -1))
	{
		StopPlay();
	}	
	//m_pM3u8List.handleVideo(vidoebuf,bufsize,TimeStamp,bkey);
	//if(m_nType&0x01)
	{
		//printf("%d data \n",bufsize);
	}
}
bool CdevSdk::Ps_AnalyzeDataGetPacketEx()
{
	#if ANALYZEDATETYPE==VERSION_3_0_0
	if (m_nAnalyzeHandle == -1)
	{
		return false;
	}	
	#else ANALYZEDATETYPE==VERSION_4_0_0
	if (m_nAnalyzeHandlePtr == NULL)
	{
		return false;
	}	
	#endif
	unsigned int TimeStamp;
	int nh264Size = 0;
	PACKET_INFO_EX stPacket;
	char temp[16]={0};
	#if ANALYZEDATETYPE==VERSION_3_0_0
	while (AnalyzeDataGetPacketEx(m_nAnalyzeHandle, &stPacket) == 0)
	#else ANALYZEDATETYPE==VERSION_4_0_0
	while (HIKANA_GetOnePacketEx(m_nAnalyzeHandlePtr, &stPacket) == 0)
	#endif
	{
		bool bkey = false;
		if (stPacket.nPacketType == VIDEO_I_FRAME)
		{
			bkey = true;
		}
		if (stPacket.nPacketType == VIDEO_I_FRAME ||stPacket.nPacketType == VIDEO_P_FRAME||stPacket.nPacketType == VIDEO_B_FRAME)
		{
			Ps_packae_header Ps_packae_header_;
			pes_packae_header pes_packae_header_;
			char* pStart = (char *)stPacket.pPacketBuffer;
			char* pEnd = (char *)stPacket.pPacketBuffer + stPacket.dwPacketSize;
			TimeStamp = stPacket.dwTimeStamp;
			nh264Size = 0;
			#ifdef		DEBUG_FILE
			FILE * m_pFileStream;
			char filePath[128]={0};
			sprintf(filePath,"h264//%d_%d_ps.h264",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort);
			//以追加的方式打开文件流
			m_pFileStream = fopen(filePath, "ab+");
			//fwrite((uint8_t *)temp,sizeof(temp),1,m_pFileStream);
			fwrite((uint8_t *)pStart,stPacket.dwPacketSize,1,m_pFileStream);
			fflush(m_pFileStream);
			fclose(m_pFileStream);	
			#endif
			while (pStart < pEnd)
			{
				unsigned char nType = (unsigned char)*(pStart + 3) ;	
				unsigned short ndatelen = 0;

				if (nType == 0xba)
				{
					pStart = Ps_packae_header_.SetValue(pStart);
				}
				else if(nType == 0xE0)
				{
					pStart = pes_packae_header_.SetValue(pStart);
					memcpy(m_h264Buf+nh264Size,pes_packae_header_.packagedata,pes_packae_header_.packagedatalen);
					nh264Size += pes_packae_header_.packagedatalen;
				}
				else
				{
					unsigned short ndatelen = get_byte16(pStart + 4);
					pStart += ndatelen + 6; 
				}
			}
			if (nh264Size>0)
			{
				boost::shared_ptr<buffer> buffPtr(new buffer(nh264Size));
				append_data(buffPtr.get(),(uint8_t *)m_h264Buf,nh264Size);
				buffPtr->bkey = bkey;
				buffPtr->TimeStamp = TimeStamp;
				m_SendBuflist.pushBack(buffPtr);
				#ifdef		DEBUG_FILE
				FILE * m_pFileStream;
				char filePath[128]={0};
				sprintf(filePath,"h264//%d_%d.h264",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort);
				//以追加的方式打开文件流
				m_pFileStream = fopen(filePath, "ab+");
				//fwrite((uint8_t *)temp,sizeof(temp),1,m_pFileStream);
				fwrite((uint8_t *)m_h264Buf,nh264Size,1,m_pFileStream);
				fflush(m_pFileStream);
				fclose(m_pFileStream);	
				#endif
				//handleVideo((uint8_t *)m_h264Buf,nh264Size, TimeStamp,bkey);
				return true;
			}

		}
#if 0
		else if (stPacket.nPacketType == AUDIO_PACKET)
		{
			pes_packae_header pes_packae_header_;
			char* pStart = (char *)stPacket.pPacketBuffer;
			char* pEnd = (char *)stPacket.pPacketBuffer + stPacket.dwPacketSize;
			unsigned char nType = (unsigned char)*(pStart + 3) ;
			while (pStart < pEnd)
			{
				unsigned char nType = (unsigned char)*(pStart + 3) ;	
				unsigned short ndatelen = 0;

				if(nType == 0xC0)
				{
					pStart = pes_packae_header_.SetValue(pStart);
					memcpy(m_h264Buf+nh264Size,pes_packae_header_.packagedata,pes_packae_header_.packagedatalen);
					nh264Size += pes_packae_header_.packagedatalen;
				}
				else
				{
					unsigned short ndatelen = get_byte16(pStart + 4);
					pStart += ndatelen + 6; 
				}
			}
			TimeStamp = stPacket.dwTimeStamp;
			if (nh264Size>0)
			{
				char buf[4096] = {0};
				int npcmOutsize;

				if (m_nVideoType == AUDIO_G722_1)
				{
					npcmOutsize = 1280;
					NET_DVR_DecodeG722Frame(m_pG722,(unsigned char*)m_h264Buf, (unsigned char*)buf);
					if (npcmOutsize > 0)
					{
						boost::shared_ptr<buffer> Audiobuf(new buffer);
						append_data( (buffer*)Audiobuf.get(),(unsigned char*)buf,1280);
						Audiobuf->TimeStamp = TimeStamp;
						m_AudioBuflist.pushBack(Audiobuf);
					}
				}
				else if (m_nVideoType == AUDIO_G711_A)
				{
					g711a_Decode((unsigned char*)m_h264Buf,(unsigned char*)buf,nh264Size,(unsigned int*)&npcmOutsize);
					if (npcmOutsize > 0)
					{
						boost::shared_ptr<buffer> Audiobuf(new buffer);
						append_data( (buffer*)Audiobuf.get(),(unsigned char*)buf,npcmOutsize);
						Audiobuf->TimeStamp = TimeStamp;
						m_AudioBuflist.pushBack(Audiobuf);
					}
				}
				else if (m_nVideoType == AUDIO_G711_U)
				{
					g711u_Decode((unsigned char*)m_h264Buf,(unsigned char*)buf,nh264Size,(unsigned int*)&npcmOutsize);
					if (npcmOutsize > 0)
					{
						boost::shared_ptr<buffer> Audiobuf(new buffer);
						append_data( (buffer*)Audiobuf.get(),(unsigned char*)buf,npcmOutsize);
						Audiobuf->TimeStamp = TimeStamp;
						m_AudioBuflist.pushBack(Audiobuf);
					}
				}
				return true;
			}
		}
#endif
	}
	return false;
}

void CdevSdk::StartRtspServerThread()
{
	m_watchVariable=0;
	m_rtspServerStart=true;
	if(m_rtspEndFlag==0)
	{
		//boost::thread thrd(&CdevSdk::runRtspServerActivity);
		m_rtspServerThread.StartThread(boost::bind(&CdevSdk::runRtspServerActivity,this));
	}	
}
void CdevSdk::stopRtspServerThread()
{
	m_watchVariable=1;
	m_rtspServerStart=false;
}

#define BUFSIZE 1024*1024*1

void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,char const* streamName,int tcpPort)//显示RTSP连接信息
{
	char* url = rtspServer->rtspURL(sms);
	UsageEnvironment& env = rtspServer->envir();
	g_logger.TraceInfo("TcpServer %d,this stream using the URL :%s",tcpPort,url);
	delete[] url;
}
int CdevSdk::startRtspServer() 
{
	if(m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort<0)
		return -1;
	//设置环境
	UsageEnvironment* env;
	if(m_CdevSdkParam.m_CdevChannelDeviceParam.m_sLocalIpaddr!="")
	{
		NetAddressList addresses(m_CdevSdkParam.m_CdevChannelDeviceParam.m_sLocalIpaddr.c_str());
		if (addresses.numAddresses() != 0) {
			SendingInterfaceAddr = *(unsigned*)(addresses.firstAddress()->data());
			ReceivingInterfaceAddr = *(unsigned*)(addresses.firstAddress()->data());
		}
	}
	OutPacketBuffer::maxSize = 500000; // allow for some possibly large H.264 frames
	Boolean reuseFirstSource = true;//如果为“true”则其他接入的客户端跟第一个客户端看到一样的视频流，否则其他客户端接入的时候将重新播放
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	//创建RTSP服务器
	UserAuthenticationDatabase* authDB = NULL;
	//authDB = new UserAuthenticationDatabase;
	//authDB->addUserRecord("admin", "12345"); // replace these with real strings
	RTSPServer* rtspServer=NULL;
	//while(1)
	{
		rtspServer = RTSPServer::createNew(*env,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort, authDB);
		if (rtspServer == NULL) {
			Sleep(1000);
			g_logger.TraceInfo("Failed to create RTSP prot:%d server: %s",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort,env->getResultMsg());
			return 0;
			//m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort++;
		}
		//else
		//	break;
		//Sleep(1000);
	}
	m_DeviceServer.setCdevSdkParam(m_CdevSdkParam,this);
	char const* descriptionString= "Media Presentation";
	char const* streamName = "ch1/main/av_stream";
	ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName, NULL,descriptionString,False);
	char const* streamNameClientDemo = "mpeg4/ch01/main/av_stream";
	ServerMediaSession* smsClientDemo = ServerMediaSession::createNew(*env, streamNameClientDemo, NULL,descriptionString,False);
	sms->addSubsession(H264LiveVideoServerMediaSubssion::createNew(*env, reuseFirstSource,this));//修改为自己实现的H264LiveVideoServerMediaSubssion
	smsClientDemo->addSubsession(H264LiveVideoServerMediaSubssion::createNew(*env, reuseFirstSource,this));//修改为自己实现的H264LiveVideoServerMediaSubssion
	rtspServer->addServerMediaSession(sms);
	rtspServer->addServerMediaSession(smsClientDemo);
	announceStream(rtspServer, sms, streamName,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort);//提示用户输入连接信息
	announceStream(rtspServer, smsClientDemo, streamNameClientDemo,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort);//提示用户输入连接信息
	try
	{		
		env->taskScheduler().doEventLoop(&m_watchVariable); //循环等待连接
	}
	catch(...)
	{
		m_rtspEndFlag=1;
		g_logger.TraceInfo("env->taskScheduler().doEventLoop is %s error no %d\n",env->getResultMsg(),env->getErrno(),m_watchVariable);
	}
	g_logger.TraceInfo("tcpserver:%d rtspServer:%d is stop--\n",m_CdevSdkParam.m_CdevChannelDeviceParam.m_nPlatDevPort,m_CdevSdkParam.m_CdevChannelDeviceParam.m_nRtspServerStartPort);
	rtspServer->removeServerMediaSession(sms);  
	rtspServer->removeServerMediaSession(smsClientDemo); 
    Medium::close(rtspServer);  
    env->reclaim();
	env = NULL;
	delete scheduler; 
	scheduler = NULL;
	return 0;
}
extern int TestStartRtspServer(int channel);
void CdevSdk::runRtspServerActivity()
{
#if 0
	m_watchVariable=0;
	m_rtspEndFlag=1;
	TestStartRtspServer(0);
	m_watchVariable=1;
	m_rtspEndFlag=0;
#else
	m_watchVariable=0;
	m_rtspEndFlag=1;
	startRtspServer();
	m_rtspServerThread.StopThread();
	m_watchVariable=1;
	m_rtspEndFlag=0;
	Sleep(1000);
#endif
}
void CdevSdk::GetKeyFrame()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
	m_rtspTime=GetTickCount();
	m_firstKey=true;
}
