#include "H264LiveVideoServerMediaSubssion.hh"


H264LiveVideoServerMediaSubssion* H264LiveVideoServerMediaSubssion::createNew(UsageEnvironment& env, Boolean reuseFirstSource, CdevSdk *ptCdevSdk)
{
	return new H264LiveVideoServerMediaSubssion(env, reuseFirstSource,ptCdevSdk);
}

H264LiveVideoServerMediaSubssion::H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource,CdevSdk *ptCdevSdk)
: H264VideoFileServerMediaSubsession(env, fFileName, reuseFirstSource)//H264VideoFileServerMediaSubsession����������Ҫ�޸ĵ��ļ���
																	  //����������Ҫ��������ʼ�����ǵĺ�����
																	  //���Ը����������ȥ����
{
	//Server_datasize = datasize;//��������Сָ��
	//Server_databuf = databuf;//������ָ��
	//Server_dosent = dosent;//���ͱ�ʾ
	m_ptCdevSdk=ptCdevSdk;
}


H264LiveVideoServerMediaSubssion::~H264LiveVideoServerMediaSubssion()
{

}

FramedSource* H264LiveVideoServerMediaSubssion::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
	/* Remain to do : assign estBitrate */
	estBitrate = 1000; // kbps, estimate
	
	//������ƵԴ
	H264FramedLiveSource* liveSource = H264FramedLiveSource::createNew(envir(),m_ptCdevSdk);
	if (liveSource == NULL)
	{
		return NULL;
	}

	// Create a framer for the Video Elementary Stream:
	return H264VideoStreamFramer::createNew(envir(), liveSource);
}
#if 0
char const* H264LiveVideoServerMediaSubssion::sdpLines()  
{  
	return fSDPLines =    
		"v=0\r\n"
		"o=- 1433692397690000 1433692397690000 IN IP4 192.168.16.130\r\n"
		"s=Media Presentation\r\n"
		"e=NONE\r\n"
		"b=AS:5050\r\n"
		"t=0 0\r\n"
		"a=control:*"
		"m=video 0 RTP/AVP 96\r\n"
		"c=IN IP4 0.0.0.0\r\n"
		"b=AS:5000\r\n"
		"a=recvonly\r\n"
		"a=control:trackID=1\r\n"
		"a=rtpmap:96 H264/90000\r\n"
		"a=fmtp:96 profile-level-id=420029; packetization-mode=1; sprop-parameter-sets=Z00AH5pkAoAt/zUBAQFAAAD6AAAdTAE=,aO48gA==\r\n"
		"a=Media_header:MEDIAINFO=494D4B48010100000400010000000000000000000000000000000000000000000000000000000000;\r\n"
		"a=appversion:1.0\r\n";
}  
#endif