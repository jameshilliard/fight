#ifndef _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#include "H264FramedLiveSource.hh"
#include "H264VideoFileServerMediaSubsession.hh"
#include "H264VideoStreamFramer.hh"

#pragma comment(lib,"E:\\openSource\\live.2015.06.25\\live\\lib\\libBasicUsageEnvironment.lib")
#pragma comment(lib,"E:\\openSource\\live.2015.06.25\\live\\lib\\libgroupsock.lib")
#pragma comment(lib,"E:\\openSource\\live.2015.06.25\\live\\lib\\libliveMedia.lib")
#pragma comment(lib,"E:\\openSource\\live.2015.06.25\\live\\lib\\libUsageEnvironment.lib")

#if 0
#pragma comment(lib,"E:\\openSource\\live.2015.04.01\\live\\lib\\libBasicUsageEnvironment.lib")
#pragma comment(lib,"E:\\openSource\\live.2015.04.01\\live\\lib\\libgroupsock.lib")
#pragma comment(lib,"E:\\openSource\\live.2015.04.01\\live\\lib\\libliveMedia.lib")
#pragma comment(lib,"E:\\openSource\\live.2015.04.01\\live\\lib\\libUsageEnvironment.lib")


#pragma comment(lib,"E:\\openSource\\live.2015.06.09\\live\\lib\\libBasicUsageEnvironment.lib")
#pragma comment(lib,"E:\\openSource\\live.2015.06.09\\live\\lib\\libgroupsock.lib")
#pragma comment(lib,"E:\\openSource\\live.2015.06.09\\live\\lib\\libliveMedia.lib")
#pragma comment(lib,"E:\\openSource\\live.2015.06.09\\live\\lib\\libUsageEnvironment.lib")

#pragma comment(lib,"D:\\WorkSpace\\openSource\\live.2015.04.01\\live\\lib\\libBasicUsageEnvironment.lib")
#pragma comment(lib,"D:\\WorkSpace\\openSource\\live.2015.04.01\\live\\lib\\libgroupsock.lib")
#pragma comment(lib,"D:\\WorkSpace\\openSource\\live.2015.04.01\\live\\lib\\libliveMedia.lib")
#pragma comment(lib,"D:\\WorkSpace\\openSource\\live.2015.04.01\\live\\lib\\libUsageEnvironment.lib")

#endif

class H264LiveVideoServerMediaSubssion : public H264VideoFileServerMediaSubsession {

public:
	static H264LiveVideoServerMediaSubssion* createNew(UsageEnvironment& env, Boolean reuseFirstSource,CdevSdk *ptCdevSdk);

protected: // we're a virtual base class
	H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource,CdevSdk *ptCdevSdk);
	~H264LiveVideoServerMediaSubssion();

protected: // redefined virtual functions
	FramedSource* createNewStreamSource(unsigned clientSessionId,unsigned& estBitrate);
	#if 0
	virtual char const* sdpLines(); 
	#endif
public:
	CdevSdk *m_ptCdevSdk;
	char fFileName[256];
	//int Server_datasize;//数据区大小指针
	//unsigned char  *Server_databuf;//数据区指针
	//bool Server_dosent;//发送标示
};

#endif
