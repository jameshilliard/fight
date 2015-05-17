#pragma once 
#include "Thread.h"
#include <vector>
#include "x264/bytestream.h"
using namespace std;

#ifdef __cplusplus  
extern"C"{   
#endif   

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>

	// for jpeglib
#ifdef __cplusplus  
}   
#endif 
class CFFmpegEncoder
{
public:
	CFFmpegEncoder(uint32_t nWidth,uint32_t nHegint,uint32_t nFrameRate);
	~CFFmpegEncoder();
	bool InitEncoder();
	int  Encode_frame( buffer* videobuf ,bool& bkey );

public:
	uint32_t m_nWidth;
	uint32_t m_nHegint;
	uint32_t m_nFrameRate;//Ö¡ÂÊ
	AVFrame *m_frame;
private:
	__int64 m_KeyTimeStamp;

	AVCodec *m_pcodec;
	AVCodecContext *m_pCodecContext;
	
	__int64 m_nBeginTime;
};