#pragma once
#include "Thread.h"
#include <boost/asio/detail/mutex.hpp>
#include "x264/bytestream.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PACKETSIZE 1000000
#define NETMTU 200

class CTsEncoder
{
public:
	CTsEncoder();
	~CTsEncoder(void);
	void handleVideo(uint8_t* vidoebuf,uint32_t bufsize,__int64 tic,bool bkey);
	void handleAudio(uint8_t* aacbuf,uint32_t bufsize,__int64 tic);
	int  OpenFile(std::string strdir,std::string tsFileName);
	void CloseFile();
	void WritetoFile(buffer* buf);
	bool IsOpen();
	void SetTsName(std::string TsFileName);

	void PutPmt();
	void PutPat();
	void CreatePesVideoPacket(buffer *ptsbuf,uint8_t* videobuf,uint32_t videoSize,bool bkey,__int64 timestamp);
	void CreatePesAudioPacket(buffer *ptsbuf,uint8_t* audiobuf,uint32_t audioSize,__int64 timestamp);
	void PutFristTsVideo(buffer *ptsbuf,__int64 tic,uint8_t fileNum);
	void PutMiddleTsVideo(buffer *ptsbuf);
	void PutlastTsVideo(buffer *ptsbuf,uint8_t filesze);


	void PutFristTsAudio(buffer *ptsbuf,__int64 tic,uint8_t fileNum);
	void PutMiddleTsAudio(buffer *ptsbuf);
	void PutlastTsAudio(buffer *ptsbuf,uint8_t filesze);

	__int64 CheckTimeStamp(__int64 nowTimeStamp);
	std::string GetFileName();
	;

	bool m_bVideobegin;//视频是否开始写了
private:
	void ResetParam();
	
	
	boost::asio::detail::mutex mutex_Lock;

	__int64 m_lastTic;//最后一帧的时间戳
	__int64 m_lastDts; //DTS 的时间戳
	__int64 m_beginTic;//最后一帧的时间戳


	//视频参数
	uint32_t m_nWidth;
	uint32_t m_nHeight;
	uint32_t nFrameRate;

	uint8_t* m_outbuf;
	int      m_outlen;


	//音频参数

	int m_nSampleRate;//采样率
	int m_nNumChannels;//通道数

	//视频计数
	uint8_t m_VideoCount;
	//音频计数
	uint8_t m_AduioCount;
	//PAT计数
	uint8_t m_PATCount;
	//PMT计数
	uint8_t m_PMTCount;

	uint8_t m_continuity_counter ;
	uint8_t GetCounter();


	uint16_t m_nprogram_map_PID;// 总的节目ID
    uint16_t m_nVideoPID;  //视频的PID
	uint16_t m_nAudioPID;  //音频的PID
	std::string m_strFileName;

	FILE* m_file;
};
