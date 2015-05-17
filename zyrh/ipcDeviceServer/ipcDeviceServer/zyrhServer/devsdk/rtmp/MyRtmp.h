#pragma once
#include "rtmp.h"
#include <string>
#include <boost/asio/detail/mutex.hpp>
#include "../x264/bytestream.h"
#include "Thread.h"
#include "Logger.h"
#ifdef __cplusplus  
extern"C"{   
#endif   

//#include "g7221/g722.h"

	// for jpeglib
#ifdef __cplusplus  
}   
#endif 

extern Logger g_logger;
/* offsets for packed values */
#define FLV_AUDIO_SAMPLESSIZE_OFFSET 1
#define FLV_AUDIO_SAMPLERATE_OFFSET  2
#define FLV_AUDIO_CODECID_OFFSET     4

#define FLV_VIDEO_FRAMETYPE_OFFSET   4

/* bitmasks to isolate specific values */
#define FLV_AUDIO_CHANNEL_MASK    0x01
#define FLV_AUDIO_SAMPLESIZE_MASK 0x02
#define FLV_AUDIO_SAMPLERATE_MASK 0x0c
#define FLV_AUDIO_CODECID_MASK    0xf0

#define FLV_VIDEO_CODECID_MASK    0x0f
#define FLV_VIDEO_FRAMETYPE_MASK  0xf0

#define AMF_END_OF_OBJECT         0x09

enum
{
	FLV_HEADER_FLAG_HASVIDEO = 1,
	FLV_HEADER_FLAG_HASAUDIO = 4,
};

enum
{
	FLV_TAG_TYPE_AUDIO = 0x08,
	FLV_TAG_TYPE_VIDEO = 0x09,
	FLV_TAG_TYPE_META  = 0x12,
};

enum
{
	FLV_MONO   = 0,
	FLV_STEREO = 1,
};

enum
{
	FLV_SAMPLESSIZE_8BIT  = 0,
	FLV_SAMPLESSIZE_16BIT = 1 << FLV_AUDIO_SAMPLESSIZE_OFFSET,
};

enum
{
	FLV_SAMPLERATE_SPECIAL = 0, /**< signifies 5512Hz and 8000Hz in the case of NELLYMOSER */
	FLV_SAMPLERATE_11025HZ = 1 << FLV_AUDIO_SAMPLERATE_OFFSET,
	FLV_SAMPLERATE_22050HZ = 2 << FLV_AUDIO_SAMPLERATE_OFFSET,
	FLV_SAMPLERATE_44100HZ = 3 << FLV_AUDIO_SAMPLERATE_OFFSET,
};

enum
{
	FLV_CODECID_MP3 = 2 << FLV_AUDIO_CODECID_OFFSET,
	FLV_CODECID_AAC = 10<< FLV_AUDIO_CODECID_OFFSET,
};

enum
{
	FLV_CODECID_H264 = 7,
};

enum
{
	FLV_FRAME_KEY   = 1 << FLV_VIDEO_FRAMETYPE_OFFSET | 7,
	FLV_FRAME_INTER = 2 << FLV_VIDEO_FRAMETYPE_OFFSET | 7,
};


#define  ENCODERX264 0
int GetSoundSampleIndex(int nSoundRate);
int GetSoundRateIndex(int nSize);

class CMyRtmp
{
public:
	CMyRtmp();
	~CMyRtmp(void);
	void handleVideo(uint8_t* vidoedata,uint32_t nsize,__int64 TimeStamp,bool bkey);
	void handleAudio(uint8_t *pAudio, int i_size,uint8_t audioType, uint64_t NowTic,int nChannel,int nSoundRate);
	void handleAudioAac(uint8_t* aacbuf,uint32_t bufsize,__int64 timeStamp,int nChannel,int nSoundRate);
	void SetParamVideo(uint32_t nWidth,uint32_t nHeight,uint32_t nFrameRate);
	void SetParamAudio(int nSampleRate,int nNumChannels);
	bool OpenUrl(std::string url);
	void SendRtmp(buffer *pbuf,uint32_t type,__int64 nTimeStamp);
	bool SetUrl(std::string url);
	std::string GetUrl();
	void StopRtmp();
private:
	
	RTMP* m_pRtmp;
	char m_pNalSPS[256];
	unsigned int m_nSpsLen;
	char m_pNalPPS[256];
	unsigned int m_nPpsLen;
	bool m_bVideobegin;//��Ƶ�Ƿ�ʼд��
	bool m_bSetparamVideo;//�Ƿ�д����Video param
	bool m_bSetparamAudio;//�Ƿ�д����Audio param
	bool m_bVideoHead;//�Ƿ�д������Ƶͷ��
	bool m_bAudioHead;//�Ƿ�д������Ƶͷ��
	uint8_t* m_outbuf;
	int      m_outlen;
	__int64  m_nLastTic;
	__int64  m_nLsatVideoTic;
	__int64  m_nNowTic;
	//��Ƶ����
	uint32_t m_nWidth;
	uint32_t m_nHeight;
	uint32_t m_nFrameRate;

	//��Ƶ����
	int m_nSampleRate;//������
	int m_nNumChannels;//ͨ����
	int m_nVideoType;

	__int64  m_nLastAudioTic;
	__int64  m_nNowAudioTic;

	//url 
	std::string m_strUrl;

	boost::asio::detail::mutex mutex_Lock;

	char* m_h264Buf;
	uint32_t m_outPslen;
	unsigned int m_PsLastTic;
private:
	
	void ResetParam();
	__int64 CheckTimeStamp(__int64 nowTimeStamp);
	__int64 CheckTimeStampAudio(__int64 nowTimeStamp);
	__int64 CheckTimeStampOffset(__int64 nowTimeStamp );

	//��Ƶ
	int WriteVidoeHeaders(char* pNalSPS,unsigned int nSpsLen,char* pNalPPS,unsigned int nPpsLen);
	void WriteVideoFrame(uint8_t *p_nalu, int i_size, bool key,uint64_t NowTic,__int64 offset);
    //����
	void WriteAudioHeaders(int nSoundRate, int nChannel);
	void WriteAacFrame(uint8_t *p_aac, int i_size, uint64_t NowTic,int nChannel,int nSoundRate);
	void WriteAudioFrame(uint8_t *pAudio, int i_size,uint8_t audioType, uint64_t NowTic,int nChannel,int nSoundRate);


public:

	
};
