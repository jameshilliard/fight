#include "stdafx.h"
#include "ffmpegEncoder.h"
#include <atlimage.h>
extern char startcodebuf[];
CFFmpegEncoder::CFFmpegEncoder(uint32_t nWidth,uint32_t nHegint,uint32_t nFrameRate)
{
	m_pcodec = NULL;
	m_pCodecContext = NULL;
	m_nBeginTime = 0;
	m_nWidth = nWidth;
	m_nHegint = nHegint;
	m_nFrameRate = nFrameRate;
	InitEncoder();	
}
CFFmpegEncoder::~CFFmpegEncoder()
{

	if (m_pCodecContext)
	{
		avcodec_close(m_pCodecContext);
		av_free(m_pCodecContext);
		m_pCodecContext = NULL;
	}
	if (m_frame)
	{
		av_freep(&m_frame->data[0]);
		av_frame_free(&m_frame);
		m_frame = NULL;
	}
}
bool CFFmpegEncoder::InitEncoder()
{
	
	avcodec_register_all();  
	m_pcodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!m_pcodec)
	{
		return false;
	}
	m_pCodecContext = avcodec_alloc_context3(m_pcodec);
	if (!m_pCodecContext)
	{
		return false;
	}
	  /* put sample parameters */
    m_pCodecContext->bit_rate = 640000;
    /* resolution must be a multiple of two */
    m_pCodecContext->width = m_nWidth;
    m_pCodecContext->height = m_nHegint;
    /* frames per second */
    m_pCodecContext->time_base.num = 1;
	m_pCodecContext->time_base.den= 25;
    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    m_pCodecContext->gop_size = 25;
    m_pCodecContext->max_b_frames = 0;
    m_pCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
	m_pCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;


    //av_opt_set(m_pCodecContext->priv_data, "preset", "slow", 0);
	av_opt_set(m_pCodecContext->priv_data, "preset", "superfast", 0);  
	av_opt_set(m_pCodecContext->priv_data, "tune", "zerolatency", 0);  

    /* open it */
    if (avcodec_open2(m_pCodecContext, m_pcodec, NULL) < 0) {
       return false;
    }
	m_frame = av_frame_alloc();
	if (!m_frame)
	{
		return false;
	}
	m_frame->format = AV_PIX_FMT_YUV420P;
	m_frame->width  = m_pCodecContext->width;
	m_frame->height = m_pCodecContext->height;
	m_frame->pts = 1;
	int size    = avpicture_get_size(AV_PIX_FMT_YUV420P, m_nWidth, m_nHegint);
	uint8_t *picture_buf = (uint8_t *)av_malloc(size);
	int ret = avpicture_fill((AVPicture *)m_frame, picture_buf, AV_PIX_FMT_YUV420P, m_nWidth, m_nHegint); 
	if (ret < 0) {
		fprintf(stderr, "Could not allocate raw picture buffer\n");
		return false;
	}
	return true;
}

int  CFFmpegEncoder::Encode_frame( buffer* videobuf,bool& bkey )
{
	//GetDesktopYuv(m_frame);

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	int got_output = 0;
	int ret = avcodec_encode_video2(m_pCodecContext, &pkt, m_frame, &got_output);
	if (ret < 0) {
		fprintf(stderr, "Error encoding frame\n");		 
	}
	if (got_output) {
		append_data(videobuf,pkt.data,pkt.size);
		bkey = pkt.flags;
	}
	av_free_packet(&pkt);
	m_frame->pts++;
	
	return videobuf->d_cur;
}
