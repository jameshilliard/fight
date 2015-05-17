#include "MyRtmp.h"
#include <x264/bytestream.h>
#include <x264/H264FrameParser.h>

CMyRtmp::~CMyRtmp(void)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	RTMP_Free(m_pRtmp);
	delete[] m_outbuf;
	WSACleanup();
}
CMyRtmp::CMyRtmp()
{
	m_pRtmp = RTMP_Alloc();
	RTMP_Init(m_pRtmp);
	m_outbuf = new uint8_t[1000*500];
	ResetParam();
	WSADATA WSAData;
	WSAStartup( MAKEWORD( 1, 1 ), &WSAData );//³õÊ¼»¯
}
void CMyRtmp::StopRtmp()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if (m_pRtmp != NULL)
	{
		RTMP_Close(m_pRtmp);	
		g_logger.TraceInfo("RTMP_Close(m_pRtmp) %s",m_strUrl.c_str());
	}
	RTMP_Init(m_pRtmp);
	g_logger.TraceInfo("StopRtmp %s",m_strUrl.c_str());
}
void CMyRtmp::SetParamVideo(uint32_t nWidth,uint32_t nHeight,uint32_t nFrameRate)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nFrameRate = nFrameRate;
	m_bSetparamVideo = true;

}
void CMyRtmp::SetParamAudio(int nSampleRate,int nNumChannels)
{
	m_nSampleRate = nSampleRate;
	m_nNumChannels = nNumChannels;
	m_bSetparamAudio = true;

}
void CMyRtmp::ResetParam()
{
	m_bVideobegin = false;
	m_bSetparamVideo = false;
	m_bSetparamAudio = false;
	m_bVideoHead = false;
	m_bAudioHead = false;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFrameRate = 0;
	m_nPpsLen = 0;
	m_nSpsLen = 0;
	m_outlen = 0;
	
	m_nSampleRate = 0;
	m_nNumChannels = 0;
	m_outPslen = 0;
	m_nLastAudioTic = 0;
	m_nNowAudioTic = 0;
	m_nLastTic = 0;

}
void CMyRtmp::handleVideo(uint8_t* vidoebuf,uint32_t bufsize,__int64 TimeStamp,bool bkey)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if (!m_pRtmp)
	{
		return;
	}
	if (!RTMP_IsConnected(m_pRtmp))
	{
		//ResetParam();
		if (bkey)
		{
			StopRtmp();
			m_bVideobegin = false;
			m_bSetparamVideo = false;
			m_bSetparamAudio = false;
			m_bVideoHead = false;
			m_bAudioHead = false;
			m_nLastAudioTic = 0;
			m_nNowAudioTic = 0;
			m_nLastTic = 0;
			if(!OpenUrl(m_strUrl))
			{
				return;
			}
		}
		else
		{
			return;
		}
		
	}
	if (!m_bVideobegin)
	{
		if (bkey)
		{
			ParseH264NalSpsPps((char*)vidoebuf,bufsize,m_pNalSPS,m_nSpsLen,m_pNalPPS,m_nPpsLen);
			if (m_nPpsLen > 0)
			{
				parse_sps((byte*)m_pNalSPS,m_nSpsLen,&m_nWidth,&m_nHeight);
			}
		}	
		else
		{
			return;
		}
		WriteVidoeHeaders(m_pNalSPS,m_nSpsLen,m_pNalPPS,m_nPpsLen);
		m_bVideobegin = true;
		m_bVideoHead = true;
		__int64 nowTs = CheckTimeStamp(TimeStamp);
		__int64 offset =0; //CheckTimeStampOffset(TimeStamp);

		printf("%I64d %s\n",nowTs,m_strUrl.c_str());
		g_logger.TraceInfo("WriteVideoFrame url:%s,m_nWidth:%d,m_nHeight:%d,",m_strUrl.c_str(),m_nWidth,m_nHeight);
		WriteVideoFrame(m_outbuf,m_outlen,bkey,nowTs,offset);
	}
	else if( m_bVideoHead)
	{
		__int64 nowTs = CheckTimeStamp(TimeStamp);
		__int64 offset =0; //CheckTimeStampOffset(TimeStamp);
		//printf("%I64d %s\n",nowTs,m_strUrl.c_str());
		CheckH264key((char*)vidoebuf,bufsize,bkey);
		AVCParseNalUnits((char*)vidoebuf,bufsize,(char*)m_outbuf,(int*)&m_outlen);
		WriteVideoFrame(m_outbuf,m_outlen,bkey,nowTs,offset);
	} 

}
void CMyRtmp::handleAudio(uint8_t *pAudio, int i_size,uint8_t audioType, uint64_t NowTic,int nChannel,int nSoundRate)
{
	return;
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if (!m_pRtmp)
	{
		return;
	}
	if (m_bVideobegin)
	{
		NowTic = CheckTimeStampAudio(NowTic);
		WriteAudioFrame(pAudio,i_size,audioType,NowTic,nChannel,nSoundRate);
	}
}
int CMyRtmp::WriteVidoeHeaders(char* pNalSPS,unsigned int nSpsLen,char* pNalPPS,unsigned int nPpsLen)
{

	buffer * pHeadBuf = new buffer(1024);
	// SEI
	/* It is within the spec to write this as-is but for
	* mplayer/ffmpeg playback this is deferred until before the first frame */

	// SPS
	uint8_t *sps = (uint8_t *)pNalSPS;

	put_byte( pHeadBuf, 7 | FLV_FRAME_KEY ); // Frametype and CodecID
	put_byte( pHeadBuf, 0 ); // AVC sequence header
	put_be24( pHeadBuf, 0 ); // composition time

	put_byte( pHeadBuf, 1 );      // version
	put_byte( pHeadBuf, sps[1] ); // profile
	put_byte( pHeadBuf, sps[2] ); // profile
	put_byte( pHeadBuf, sps[3] ); // level
	put_byte( pHeadBuf, 0xff );   // 6 bits reserved (111111) + 2 bits nal size length - 1 (11)
	put_byte( pHeadBuf, 0xe1 );   // 3 bits reserved (111) + 5 bits number of sps (00001)

	put_be16( pHeadBuf, nSpsLen  );
	append_data( pHeadBuf, sps, nSpsLen  );

	// PPS
	put_byte( pHeadBuf, 1 ); // number of pps
	put_be16( pHeadBuf, nPpsLen  );
	append_data( (buffer*)pHeadBuf, (uint8_t*)pNalPPS , (uint32_t)nPpsLen );

	// rewrite data length info
	SendRtmp(pHeadBuf,FLV_TAG_TYPE_VIDEO,0);
	delete pHeadBuf;
	return 1;
}
void CMyRtmp::SendRtmp(buffer *pbuf,uint32_t type,__int64 nTimeStamp)
{
	RTMPPacket rtmpPacket;
	RTMPPacket_Reset(&rtmpPacket);
	RTMPPacket_Alloc(&rtmpPacket,pbuf->d_cur);

	//printf("%I64d %s\n",nTimeStamp,m_strUrl.c_str());
	rtmpPacket.m_packetType = type;
	rtmpPacket.m_nBodySize = pbuf->d_cur;
	rtmpPacket.m_nTimeStamp = nTimeStamp;
	rtmpPacket.m_nChannel = 4;
	rtmpPacket.m_headerType = RTMP_PACKET_SIZE_LARGE;
	rtmpPacket.m_packetType = type;
	rtmpPacket.m_nInfoField2 = m_pRtmp->m_stream_id;

	memcpy(rtmpPacket.m_body,pbuf->data,pbuf->d_cur);


	int ret1 = RTMP_IsConnected(m_pRtmp);
	//int ret2 = RTMP_IsTimedout(m_pRtmp);
	if (  ret1 <= 0 )
	{
// 		ResetParam();
// 		RTMP_Close(m_pRtmp);
// 		RTMP_Init(m_pRtmp);
// 		OpenUrl(m_strUrl);	
		RTMPPacket_Free(&rtmpPacket);
		return;
	}
	int ret = RTMP_SendPacket(m_pRtmp,&rtmpPacket,0);
	RTMPPacket_Free(&rtmpPacket);

}
bool CMyRtmp::OpenUrl(std::string url)
{
	ResetParam();
	bool bret = false;
	m_strUrl = url;
	g_logger.TraceInfo("connect OpenUrl %s,",m_strUrl.c_str());
	int err= RTMP_SetupURL(m_pRtmp,(char*)url.c_str());
	if (err <= 0)
	{
		return bret;
	}
	RTMP_EnableWrite(m_pRtmp);
	err = RTMP_Connect(m_pRtmp,NULL);
	if (err <= 0)
	{
		return bret;
	}
	err = RTMP_ConnectStream(m_pRtmp,0);
	if (err <= 0)
	{
		return bret;
	}
	bret = true;
	m_strUrl = url;
	g_logger.TraceInfo("connect OpenUrl %s,bret = true",m_strUrl.c_str());
	return bret;
}
bool CMyRtmp::SetUrl(std::string url)
{
	m_strUrl = url;
	return true;
}
void CMyRtmp::WriteVideoFrame(uint8_t *p_nalu, int i_size, bool key,uint64_t NowTic,__int64 offset)
{
	buffer *c = new buffer(1024);

	// A new frame - write packet header
	//put_byte( c, FLV_TAG_TYPE_VIDEO );

	put_byte( c, key ? FLV_FRAME_KEY : FLV_FRAME_INTER );
	put_byte( c, 1 ); // AVC NALU
	put_be24( c, offset );


	append_data( c, p_nalu, i_size );

	SendRtmp(c,FLV_TAG_TYPE_VIDEO,NowTic);
	delete c;
}
__int64 CMyRtmp::CheckTimeStamp(__int64 nowTimeStamp)
{

	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);

	if (m_nLastTic == 0)
	{
		m_nLastTic = nowTimeStamp;
		m_nNowTic = 0;

	}
	else
	{
		if (m_nLastTic >= nowTimeStamp)
		{
			m_nNowTic += 50;
		}
		else
		{
			__int64 ndif = nowTimeStamp - m_nLastTic;
// 			if (ndif>100)
// 			{
// 				ndif = ndif*0.9;
// 			}
			m_nNowTic += ndif;
		}
		m_nLastTic = nowTimeStamp;
// 		if (m_nNowTic <= m_nNowAudioTic)
// 		{
// 			m_nNowTic = m_nNowAudioTic + 10;
// 		}
	}
	return m_nNowTic;
}
__int64 CMyRtmp::CheckTimeStampAudio(__int64 nowTimeStamp)
{

	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);

	if (m_nLastAudioTic == 0)
	{
		m_nLastAudioTic = nowTimeStamp;
		m_nNowAudioTic = 0;

	}
	else
	{
		if (m_nLastAudioTic >= nowTimeStamp)
		{
			m_nNowAudioTic += 50;
		}
		else
		{
			m_nNowAudioTic += (nowTimeStamp - m_nLastAudioTic);
		}
		m_nLastAudioTic = nowTimeStamp;
// 		if (m_nNowAudioTic  <= m_nNowTic)
// 		{
// 			m_nNowAudioTic  = m_nNowTic + 10;
// 		}
	}
	return m_nNowAudioTic;
}
__int64 CMyRtmp::CheckTimeStampOffset(__int64 nowTimeStamp )
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	__int64 retTs;
	if (m_nLsatVideoTic == 0)
	{
		m_nLsatVideoTic = nowTimeStamp;
		retTs = m_nLsatVideoTic;
	}
	else
	{
		retTs = nowTimeStamp - m_nLsatVideoTic;
		m_nLsatVideoTic = nowTimeStamp;
	}
	return retTs;
}
void CMyRtmp::WriteAudioHeaders(int nSoundRate, int nChannel)
{
	buffer *c = new buffer(1024);

	uint8_t nSoundRateIndex = GetSoundRateIndex(nSoundRate);

	//put AUDIODATA 
	uint8_t flag = 10<<4 | //10 = AAC 
		nSoundRateIndex<<2  //0 = 5.5-kHz  1 = 11-kHz 2 = 22-kHz 3 = 44-kHz 
		| 1<<1 //Size of each sample   For AAC: always 1 
		| 1;  //Mono or stereo sound  For AAC: always 1 
	put_byte( c, flag );

	put_byte( c, 0);//0: AAC sequence header
	//put AudioSpecificConfig
	int nSampleIndex = GetSoundSampleIndex(nSoundRate); 
	uint16_t AudioSpecificConfig = 2<<11 |nSampleIndex<<7|nChannel<<3;
	put_be16( c, AudioSpecificConfig );

	SendRtmp(c,FLV_TAG_TYPE_AUDIO,0);
	delete c;
}
void CMyRtmp::WriteAacFrame(uint8_t *p_aac, int i_size, uint64_t NowTic,int nChannel,int nSoundRate)
{
	// A new frame - write packet header
	buffer *c = new buffer(1024);

	uint8_t nSoundRateIndex = GetSoundRateIndex(nSoundRate);

	//put AUDIODATA 
	uint8_t flag = 10<<4 | //10 = AAC 
		3<<2  //0 = 5.5-kHz  1 = 11-kHz 2 = 22-kHz 3 = 44-kHz 
		| 1<<1 //Size of each sample   For AAC: always 1 
		| 1;  //Mono or stereo sound  For AAC: always 1 
	put_byte( c, flag );

	put_byte( c, 1);//1: AAC raw 
	append_data( c, p_aac, i_size );// Raw AAC frame data 
	SendRtmp(c,FLV_TAG_TYPE_AUDIO,NowTic);
	delete c;
}
int GetSoundSampleIndex(int nSoundRate)
{
	int nSampleIndex = 0x07;
	switch (nSoundRate)
	{
	case 96000:
		nSampleIndex = 0x00;	
		break;
	case 88200:
		nSampleIndex = 0x01;	
		break;
	case 64000:
		nSampleIndex = 0x02;	
		break;
	case 48000:
		nSampleIndex = 0x03;	
		break;
	case 44100:
		nSampleIndex = 0x04;	
		break;
	case 32000:
		nSampleIndex = 0x05;	
		break;
	case 24000:
		nSampleIndex = 0x06;	
		break;
	case 22050:
		nSampleIndex = 0x07;	
		break;
	case 16000:
		nSampleIndex = 0x08;	
		break;
	case 12000:
		nSampleIndex = 0x09;	
		break;
	case 11025:
		nSampleIndex = 0x0A;	
		break;
	case 8000:
		nSampleIndex = 0x0B;	
		break;
	case 7350:
		nSampleIndex = 0x0C;	
		break;
	}
	return nSampleIndex;
}
int GetSoundRateIndex(int nSize)
{
	if (nSize <= 8000)
		return 0;
	if (nSize > 8000 && nSize <= 16000)
		return 1;
	if (nSize > 16000 && nSize <= 32000)
		return 2;
	else
		return 3;
}

void CMyRtmp::WriteAudioFrame(uint8_t *pAudio, int i_size,uint8_t audioType, uint64_t NowTic,int nChannel,int nSoundRate)
{
	buffer *c = new buffer(1024);

	uint8_t nSoundRateIndex = GetSoundRateIndex(nSoundRate);

	unsigned char sndBit = 0;
	switch (audioType)
	{
	case 8:
		sndBit = 1;
		break;
	case 7:
		sndBit = 1;
		break;
	default:
		sndBit = 1;
		break;
	}

	//put AUDIODATA 
	uint8_t flag = audioType<<4 | //10 = AAC 
		nSoundRateIndex<<2  //0 = 5.5-kHz  1 = 11-kHz 2 = 22-kHz 3 = 44-kHz 
		|1<<1 //Size of each sample   For AAC: always 1 0 = snd8Bit 1 = snd16Bit
		| 0;  //Mono or stereo sound  For AAC: always 1 
	put_byte( c, flag );

	append_data( c, pAudio, i_size );// Raw AAC frame data 
	SendRtmp(c,FLV_TAG_TYPE_AUDIO,NowTic);
	delete c;

}


void CMyRtmp::handleAudioAac(uint8_t* aacbuf,uint32_t bufsize,__int64 timeStamp,int nChannel,int nSoundRate)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if (m_bVideobegin)
	{
		if (!m_bAudioHead)
		{
			m_nSampleRate = nSoundRate;
			m_nNumChannels = nChannel;
			WriteAudioHeaders(m_nSampleRate,m_nNumChannels);
			m_bAudioHead = true;
		}
		__int64 nowTs = CheckTimeStampAudio(timeStamp);
		WriteAacFrame(aacbuf,bufsize,nowTs,m_nNumChannels,m_nSampleRate);	
	}

}
std::string CMyRtmp::GetUrl()
{
	return m_strUrl;
}