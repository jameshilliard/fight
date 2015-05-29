#ifndef _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#include "H264FramedLiveSource.hh"
#include "H264VideoFileServerMediaSubsession.hh"
#include "H264VideoStreamFramer.hh"

#if 0
#pragma comment(lib,"G:\\OnLineProject\\openSource\\live.2015.04.01\\live\\lib\\libBasicUsageEnvironment.lib")
#pragma comment(lib,"G:\\OnLineProject\\openSource\\live.2015.04.01\\live\\lib\\libgroupsock.lib")
#pragma comment(lib,"G:\\OnLineProject\\openSource\\live.2015.04.01\\live\\lib\\libliveMedia.lib")
#pragma comment(lib,"G:\\OnLineProject\\openSource\\live.2015.04.01\\live\\lib\\libUsageEnvironment.lib")
#endif

#pragma comment(lib,"D:\\WorkSpace\\openSource\\live.2015.04.01\\live\\lib\\libBasicUsageEnvironment.lib")
#pragma comment(lib,"D:\\WorkSpace\\openSource\\live.2015.04.01\\live\\lib\\libgroupsock.lib")
#pragma comment(lib,"D:\\WorkSpace\\openSource\\live.2015.04.01\\live\\lib\\libliveMedia.lib")
#pragma comment(lib,"D:\\WorkSpace\\openSource\\live.2015.04.01\\live\\lib\\libUsageEnvironment.lib")

class H264LiveVideoServerMediaSubssion : public H264VideoFileServerMediaSubsession {

public:
	static H264LiveVideoServerMediaSubssion* createNew(UsageEnvironment& env, Boolean reuseFirstSource,CdevSdk *ptCdevSdk);

protected: // we're a virtual base class
	H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource,CdevSdk *ptCdevSdk);
	~H264LiveVideoServerMediaSubssion();

protected: // redefined virtual functions
	FramedSource* createNewStreamSource(unsigned clientSessionId,unsigned& estBitrate);
public:
	CdevSdk *m_ptCdevSdk;
	char fFileName[100];
	//int Server_datasize;//数据区大小指针
	//unsigned char  *Server_databuf;//数据区指针
	//bool Server_dosent;//发送标示
};

#endif
