#include <vector>
#include "../x264/bytestream.h"
using namespace std;
extern "C"
{
#include <x264.h>
#include <x264_config.h>
}; 
class CH264Encoder
{
public:
	CH264Encoder(uint32_t nWidth,uint32_t nHegint,uint32_t nFrameRate);
	~CH264Encoder();
	int InitEncoder();
	int Encode_frame( buffer* videobuf);

public:
	uint32_t m_nWidth;
	uint32_t m_nHegint;
	uint32_t m_nFrameRate;//Ö¡ÂÊ
private:
	x264_param_t* m_pH264_param;
	x264_t *m_pH264;
	
	__int64 m_KeyTimeStamp;
public:
	x264_picture_t *m_pPic;
};