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