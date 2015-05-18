#include "./H264FrameDeviceSource.h"
#include <iostream>
#include "WindowsPlayM4.h"
#include <GlobalClass.h>
#include <GlobalParam.h>
#include "DevManager.h"
#include "AnalyzeDataInterface.h"


#define MYSYSTEM_MPEG2_PS   0x02	// PS封装



static void CALLBACK DecCBFun(long nPort,char * pBuf,long nSize,
					   FRAME_INFO * pFrameInfo, 
					   long nReserved1,long nReserved2)
{

	//---------------------------------------
	// 获取解码后音频数据
	//boost::shared_ptr<H264FrameDeviceSource> DevPtr = CGlobalClass::GetInstance()->GetDevManager()->GetDev(nReserved1);
	H264FrameDeviceSource * DevPtr  = (H264FrameDeviceSource *)nReserved1;
	
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

static void CBF_OnStreamPlay(int h,int sflag,const char* data  ,unsigned int data_size,void* ctx)
{
	//unsigned int nIndex = (unsigned int)ctx;
	//boost::shared_ptr<H264FrameDeviceSource> DevPtr = CGlobalClass::GetInstance()->GetDevManager()->GetDev(nIndex);
	H264FrameDeviceSource * DevPtr = (H264FrameDeviceSource *)ctx;
	if (!DevPtr)
	{
		return;
	}
	DevPtr->InPutPsData((unsigned char *)data,data_size,sflag);
		
}
H264FrameDeviceSource::H264FrameDeviceSource(SDKServerData nSdkServerData)
{
	m_nSkdPlayPort = -1;
	m_h264Buf = new char[512*1024];
	m_pffmpegEncoder = NULL;
	m_pAacEncoder = NULL;
	m_pG722 = NULL;
	m_nAnalyzeHandle = -1;
	m_nTimeNow = time(NULL);
	boost::asio::io_service& _IoService = CGlobalClass::GetInstance()->GetIoservice()->get_io_service();
	
	m_io_timer_Ptr.reset(new boost::asio::deadline_timer(_IoService) );
	m_pIoService = &_IoService;
	
	m_bStop = false;

	m_nCheckDelTimeOut = 0;

	//zss++
	m_SdkServerData=nSdkServerData;
	m_sServerIp = nSdkServerData.m_sSdkServerIp;
	m_nServerPort = nSdkServerData.m_nSdkServerPort;
	m_nServerLine = nSdkServerData.m_nServerLine;
	m_spassword = nSdkServerData.m_spassword;
	m_nDevLine = nSdkServerData.m_nDevLine;;
	m_nnchannel = nSdkServerData.m_nnchannel;
	m_sDevId = nSdkServerData.m_sDevId;
	m_pM3u8List.m_sdveid = nSdkServerData.m_sDevId;
	m_stream_handle=-1;
	m_wmp_handle=-1;
	//zss--
}
H264FrameDeviceSource::~H264FrameDeviceSource()
{
	delete[] m_h264Buf;
	if (m_io_timer_Ptr)
	{
		m_io_timer_Ptr.reset();
		 
	}

}
void H264FrameDeviceSource::OnTime(const boost::system::error_code& e)
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
	m_io_timer_Ptr->async_wait(boost::bind(&H264FrameDeviceSource::OnTime,shared_from_this(),boost::asio::placeholders::error));

}
int H264FrameDeviceSource::StartDev(char* sServerIp,unsigned int nServerPort,unsigned int nServerLine,char* sdevId,char* password,unsigned int nDevLine,unsigned int nchannel,int nType)
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
	
	//zss++if (m_srtmpurl != "")
	//{
	//	if (!m_myRtmp.OpenUrl(m_srtmpurl))
	//	{
	//		return -1;
	//	}
	//zss--}
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
		CBF_OnStreamPlay, (void*)this,(int*)&m_stream_handle);
	g_logger.TraceInfo("sdk取流 设备ID:%s 设备通道号:%d,设备线路号:%d,m_wmp_handle:%d,取流返回:%d ",m_sDevId.c_str(),m_nnchannel,m_nDevLine,m_wmp_handle,ret);
	m_io_timer_Ptr->expires_from_now(boost::posix_time::seconds(5));
	m_io_timer_Ptr->async_wait(boost::bind(&H264FrameDeviceSource::OnTime,shared_from_this(),boost::asio::placeholders::error));
	return ret;
}

bool H264FrameDeviceSource::StartDev()
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
	g_logger.TraceInfo("sdk重新取流 设备ID:%s 设备通道号:%d,设备线路号:%d,m_wmp_handle:%d,取流返回:%d ",m_sDevId.c_str(),m_nnchannel,m_nDevLine,m_wmp_handle,ret);
	if (ret != 0)
	{
		return false;
	}
	return true;
}
bool H264FrameDeviceSource::ReStartDev()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
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
	g_logger.TraceInfo("sdk重新取流 设备ID:%s 设备通道号:%d,设备线路号:%d,m_wmp_handle:%d,取流返回:%d ",m_sDevId.c_str(),m_nnchannel,m_nDevLine,m_wmp_handle,ret);
	if (ret != 0)
	{
		return false;
	}
	return true;
}
void H264FrameDeviceSource::ResetParam()
{
	m_wmp_handle = -1;

}
int H264FrameDeviceSource::CmdPtzControl(std::string sdevid,unsigned int nchannel,int ptz_cmd,int action,int param,std::string& smsg )
{
	int nSdkRet = -1;
	if (m_wmp_handle == -1)
	{
		smsg = "设备未登陆";
		return nSdkRet;
	}
	printf("sdk重新取流 设备ID:%s 设备通道号:%d,云台命令:%d,云台操作:%d,云台参数:%d ",sdevid.c_str(),m_nnchannel,ptz_cmd,action,param);
	nSdkRet = WMP_PtzControl(m_wmp_handle,sdevid.c_str(),nchannel,ptz_cmd,action,param);

	smsg = "sdk ret";
	return nSdkRet;
}
void H264FrameDeviceSource::StopDev()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	m_bStop = true;
	if (m_io_timer_Ptr)
	{
		m_io_timer_Ptr->cancel();
	}
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
	if (m_nAnalyzeHandle != -1)
	{
		AnalyzeDataClose(m_nAnalyzeHandle);
		g_logger.TraceInfo("停止解包 m_nAnalyzeHandle %d devid:%s",m_nAnalyzeHandle,m_sDevId.c_str());
		m_nAnalyzeHandle = -1;
	}
	if (m_pG722)
	{
		NET_DVR_ReleaseG722Decoder(m_pG722);
		m_pG722 = NULL;
	}
	//zss++m_myRtmp.StopRtmp();

}
void H264FrameDeviceSource::StopPlay()
{
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
	if (m_nAnalyzeHandle != -1)
	{
		AnalyzeDataClose(m_nAnalyzeHandle);
		g_logger.TraceInfo("停止解包 m_nAnalyzeHandle %d devid:%s",m_nAnalyzeHandle,m_sDevId.c_str());
		m_nAnalyzeHandle = -1;
	}

}
bool H264FrameDeviceSource::addDeviceSource(std::vector<std::string > *vDeviceSource)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
	if(m_deviceSource.size()>10)
		return false;
	m_deviceSource.push_back(vDeviceSource);
	return true;
}
bool H264FrameDeviceSource::removeDeviceSource(std::vector<std::string > *vDeviceSource)
{
	unsigned int i=0;
	boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
	for (i=0; i<m_deviceSource.size(); i++)
	{
		if(m_deviceSource[i]==vDeviceSource)
		{
			m_deviceSource.erase(m_deviceSource.begin()+i);
			return true;
		}

	}
	return false;
}
bool H264FrameDeviceSource::GetVideoData(std::vector<std::string > *vDeviceSource,unsigned char *ptData,unsigned int &frameSize,unsigned int dataMaxSize,unsigned int &curVideoIndex)
{
	unsigned int i=0;
	bool bFindFlag=false;
	{
		boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
		for (i=0; i<m_deviceSource.size(); i++)
		{
			if(m_deviceSource[i]==vDeviceSource)
			{
				bFindFlag=true;
			}
		}
	}
	if(bFindFlag)
	{
		unsigned int timeOut=0;
		do
		{
			Sleep(10);
			timeOut++;
			if(timeOut>300)
				break;
			bFindFlag=vDeviceSource->empty();
		}while(bFindFlag);
		boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
		if(!vDeviceSource->empty())
		{
			std::vector<std::string >::iterator it=vDeviceSource->begin();
			frameSize=it->length();
			//printf("this time:%d,this 0x%x,fFrameSize 1 is %d--%d-%d-\n",GetTickCount(),this,frameSize,curVideoIndex,dataMaxSize);
			if(curVideoIndex!=0)
			{	
				frameSize=frameSize-curVideoIndex;
			}
			if(frameSize<=dataMaxSize)
			{
				memcpy(ptData,it->c_str()+curVideoIndex,frameSize);
				curVideoIndex=0;
			}	
			else if(dataMaxSize>0)
			{	
				memcpy(ptData,it->c_str()+curVideoIndex,dataMaxSize);
				frameSize=dataMaxSize;
				curVideoIndex=curVideoIndex+dataMaxSize;
			}
			if(curVideoIndex==0)
			{
				vDeviceSource->erase(it);
				return true;
			}
			else
				return false;
			
		}
		else
		{
			frameSize=0;
			return false;
		}
	}
	else
		return false;
	
}

std::string H264FrameDeviceSource::CreateM3u8File()
{
	return m_pM3u8List.CreateM3u8File();
}
bool H264FrameDeviceSource::InPutPsData(unsigned char* videoPsBuf,unsigned int  psBufsize,int nType )
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
			m_nAnalyzeHandle = AnalyzeDataGetSafeHandle();
			AnalyzeDataOpenStreamEx(m_nAnalyzeHandle, (PBYTE)videoPsBuf);

		}
		else if(m_nSkdPlayPort == -1)
		{
			PlayM4_GetPort(&m_nSkdPlayPort);
			//设置流模式类型 
			PlayM4_SetStreamOpenMode(m_nSkdPlayPort,STREAME_REALTIME); 
			//打开流模式
			PlayM4_OpenStream(m_nSkdPlayPort,(PBYTE)videoPsBuf,psBufsize,1024 * 1024);

			PlayM4_SetDecCallBackExMend(m_nSkdPlayPort,DecCBFun,NULL,0,(long)((void*)this));
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
			AnalyzeDataInputData(m_nAnalyzeHandle, videoPsBuf, psBufsize);
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
void H264FrameDeviceSource::StartVideoEncoderThread()
{
	m_pThread.StartThread(boost::bind(&H264FrameDeviceSource::handleEncoder,this));
}
void H264FrameDeviceSource::StartAudioEncoderThread()
{
	m_pThreadAudio.StartThread(boost::bind(&H264FrameDeviceSource::handleEncoderAudio,this));
}
void H264FrameDeviceSource::handleEncoder()
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
		g_logger.TraceInfo("handleEncoder video data");
		nTime = time(NULL);
	}
	if (m_VideoBuflist.GetSize() > 50)
	{
		g_logger.TraceInfo("%s m_VideoBuflist.clear  %d",m_sDevId.c_str(),m_VideoBuflist.GetSize());
		m_VideoBuflist.clear();
		m_AudioBuflist.clear();
	}

}
void H264FrameDeviceSource::handleEncoderAudio()
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
void H264FrameDeviceSource::handleAudioAac(uint8_t* aacbuf,uint32_t bufsize,__int64 timeStamp,int nChannel,int nSoundRate)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
	//m_pM3u8List.handleAudio(aacbuf,bufsize,timeStamp);
	//if(m_nType&0x01)
	{
		//zss++m_myRtmp.handleAudioAac(aacbuf+7,bufsize-7,timeStamp,nChannel,nSoundRate);
	}
	
	

}
void H264FrameDeviceSource::handleVideo(uint8_t* vidoebuf,uint32_t bufsize,__int64 TimeStamp,bool bkey)
{
	unsigned int getVideoFlag=0;
	unsigned int i=0,size=0;
	boost::asio::detail::mutex::scoped_lock lock(mutex_HandleVideo);
	std::string temp((char *)vidoebuf,bufsize);
	size=m_deviceSource.size();
	for (i=0; i<size; i++)
	{
		//if(m_deviceSource[i]->size()<200)
		{
			m_deviceSource[i]->push_back(temp);
			getVideoFlag=1;
			//printf("this 0x%x,insert data size is %d   stask %d\n",this,temp.length(),m_deviceSource[i]->size());
		}
	}
	if(getVideoFlag==0)
		StopPlay();
	//m_pM3u8List.handleVideo(vidoebuf,bufsize,TimeStamp,bkey);
	//if(m_nType&0x01)
	{
		//printf("%d data \n",bufsize);
		//zss++m_myRtmp.handleVideo(vidoebuf,bufsize,TimeStamp,bkey);
	}
}
bool H264FrameDeviceSource::Ps_AnalyzeDataGetPacketEx()
{
	if (m_nAnalyzeHandle == -1)
	{
		return false;
	}
	unsigned int TimeStamp;
	int nh264Size = 0;
	PACKET_INFO_EX stPacket;
	while (AnalyzeDataGetPacketEx(m_nAnalyzeHandle, &stPacket) == 0)
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
			char* pStart = stPacket.pPacketBuffer;
			char* pEnd = stPacket.pPacketBuffer + stPacket.dwPacketSize;
			TimeStamp = stPacket.dwTimeStamp;
			nh264Size = 0;
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
				//handleVideo((uint8_t *)m_h264Buf,nh264Size, TimeStamp,bkey);
				return true;
			}

		}
		else if (stPacket.nPacketType == AUDIO_PACKET)
		{
			pes_packae_header pes_packae_header_;
			char* pStart = stPacket.pPacketBuffer;
			char* pEnd = stPacket.pPacketBuffer + stPacket.dwPacketSize;
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
	}
	return false;
}
