#ifndef _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#include "H264FramedLiveSource.hh"
#include "H264VideoFileServerMediaSubsession.hh"
#include "H264VideoStreamFramer.hh"

#pragma comment(lib,"G:\\OnLineProject\\openSource\\live.2015.04.01\\live\\lib\\libBasicUsageEnvironment.lib")
#pragma comment(lib,"G:\\OnLineProject\\openSource\\live.2015.04.01\\live\\lib\\libgroupsock.lib")
#pragma comment(lib,"G:\\OnLineProject\\openSource\\live.2015.04.01\\live\\lib\\libliveMedia.lib")
#pragma comment(lib,"G:\\OnLineProject\\openSource\\live.2015.04.01\\live\\lib\\libUsageEnvironment.lib")

class H264LiveVideoServerMediaSubssion : public H264VideoFileServerMediaSubsession {

public:
	static H264LiveVideoServerMediaSubssion* createNew(UsageEnvironment& env, Boolean reuseFirstSource,H264FrameDeviceSource *ptH264FrameDeviceSource);

protected: // we're a virtual base class
	H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource,H264FrameDeviceSource *ptH264FrameDeviceSource);
	~H264LiveVideoServerMediaSubssion();

protected: // redefined virtual functions
	FramedSource* createNewStreamSource(unsigned clientSessionId,unsigned& estBitrate);
public:
	H264FrameDeviceSource *m_ptH264FrameDeviceSource;
	char fFileName[100];
	//int Server_datasize;//��������Сָ��
	//unsigned char  *Server_databuf;//������ָ��
	//bool Server_dosent;//���ͱ�ʾ
};

#endif