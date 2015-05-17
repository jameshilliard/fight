#include "stdafx.h"
#include "H264Encoder.h"
#include <atlimage.h>


char startcodebuf[] = {0x00, 0x00, 0x00, 0x01};
CH264Encoder::CH264Encoder(uint32_t nWidth,uint32_t nHegint,uint32_t nFrameRate)
{
	m_nFrameRate = nFrameRate;
	m_nWidth = nWidth;
	m_nHegint = nHegint;

	m_pH264_param = new x264_param_t();
	m_pH264 = NULL;

	x264_param_default(m_pH264_param);
	m_pPic = new x264_picture_t();
	x264_picture_init(m_pPic);
	x264_picture_alloc(m_pPic, X264_CSP_I420, m_nWidth, m_nHegint );
	m_pPic->img.i_csp = X264_CSP_I420;
	m_pPic->img.i_plane = 3;
	m_pPic->i_pts = 0;
	m_pPic->i_dts = 0;
	m_KeyTimeStamp = 0;
	InitEncoder();
}
CH264Encoder::~CH264Encoder()
{
	if (m_pPic)
	{
		x264_picture_clean(m_pPic);
		delete[] m_pPic;
		m_pPic = NULL;
	}
	if (m_pH264 != NULL)
	{
		x264_encoder_close(m_pH264);
		m_pH264 = NULL;
	}
	
	if (m_pH264_param)
	{
		delete m_pH264_param;
		m_pH264_param = NULL;
	}
	

}
int CH264Encoder::InitEncoder()
{
	m_pH264_param->i_width =  m_nWidth;
	m_pH264_param->i_height = m_nHegint;

	m_pH264_param->i_fps_num = m_nFrameRate;
	m_pH264_param->i_fps_den = 1;

	m_pH264_param->rc.i_bitrate = 100;
	m_pH264_param->rc.i_rc_method = X264_RC_ABR;

	m_pH264_param->i_frame_reference = 2; /* 参考帧的最大帧数 */
	m_pH264_param->i_frame_total = 0;
	m_pH264_param->i_bframe = 0;
	m_pH264_param->i_threads = 1;
	m_pH264_param->rc.i_lookahead = 0;
	m_pH264_param->i_keyint_max = 8;
	m_pH264_param->i_keyint_min = 4;


	m_pH264_param->b_cabac = 1;
	m_pH264_param->analyse.b_transform_8x8 = 1;
	m_pH264_param->i_level_idc = 12;

	if( ( m_pH264 = x264_encoder_open( m_pH264_param ) ) == NULL )
	{
		fprintf( stderr, "x264 [error]: x264_encoder_open failed\n" );
		return -1;
	}
	return 1;
}
 int  CH264Encoder::Encode_frame( buffer* videobuf )
 {
	 x264_picture_t pic_out;
	 x264_nal_t *nal;	//NAL打包指针
	 int i_nal, i,j;
	 int i_file = 0;
	 int ret;
	 m_pPic->i_pts ++;
	 m_pPic->i_type = X264_TYPE_AUTO;
	
	 m_KeyTimeStamp = GetTickCount();
	 m_pPic->i_type = X264_TYPE_AUTO;
	 videobuf->TimeStamp = GetTickCount();
	 if( ret = x264_encoder_encode( m_pH264, &nal, &i_nal, m_pPic, &pic_out ) < 0 )			//编码一帧图像
	 {
		 fprintf( stderr, "x264 [error]: x264_encoder_encode failed\n" );
	 }
	 UINT outLen = 0;
	 for (int i = 0; i < i_nal; ++i)
	 {
		 int nal_size = 0;
		 if (nal[i].p_payload[0] == 0x00 &&
			 nal[i].p_payload[1] == 0x00 &&
			 nal[i].p_payload[2] == 0x01)
		 {
			 nal_size = nal[i].i_payload - 3;
			 append_data(videobuf,(uint8_t*)startcodebuf,4);
			 outLen += 4;
			 append_data(videobuf,(uint8_t*)nal[i].p_payload + 3,nal_size);
			 outLen += nal_size;
		 }
		 else if (nal[i].p_payload[0] == 0x00 &&
			 nal[i].p_payload[1] == 0x00 &&
			 nal[i].p_payload[2] == 0x00 &&
			 nal[i].p_payload[3] == 0x01)
		 {
			 nal_size = nal[i].i_payload - 4;
			 append_data(videobuf,(uint8_t*)startcodebuf,4);
			 outLen += 4;
			 append_data(videobuf,(uint8_t*)nal[i].p_payload + 4,nal_size);
			 outLen += nal_size;
		 }
	 }
	 return videobuf->d_cur;
 }
