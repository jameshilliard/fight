#include "H264FramedLiveSource.hh" 
 

H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment& env,CdevSdk *ptCdevSdk, unsigned preferredFrameSize, unsigned playTimePerFrame)
: FramedSource(env)
{
	//Framed_datasize = datasize;//��������Сָ��
	//Framed_databuf = databuf;//������ָ��
	//Framed_dosent = dosent;//���ͱ�ʾ
	m_ptCdevSdk=ptCdevSdk;
	m_curVideoIndex=0;
}

H264FramedLiveSource* H264FramedLiveSource::createNew(UsageEnvironment& env, CdevSdk *ptCdevSdk, unsigned preferredFrameSize, unsigned playTimePerFrame)
{
	//static H264FramedLiveSource  newSource(env, nSdkServerData, preferredFrameSize, playTimePerFrame);
	H264FramedLiveSource* newSource = new H264FramedLiveSource(env,ptCdevSdk, preferredFrameSize, playTimePerFrame);
	newSource->setDeviceSource(ptCdevSdk);
	return newSource;
}

H264FramedLiveSource::~H264FramedLiveSource()
{
	m_ptCdevSdk->removeDeviceSource(&m_h264Data);
	m_ptCdevSdk=NULL;
}


void H264FramedLiveSource::doGetNextFrame()
{
	unsigned int dataSize=0;
	unsigned int frameSize=0;
	int i=0;
	bool bRet=true;
	//while(bRet)
	{
		bRet=m_ptCdevSdk->GetVideoData(&m_h264Data,fTo+dataSize,frameSize,fMaxSize-dataSize,m_curVideoIndex);
		dataSize+=frameSize;
	}
	fFrameSize=dataSize;
	if(fFrameSize==0)
		fFrameSize=fMaxSize;
	nextTask() = envir().taskScheduler().scheduleDelayedTask(0,(TaskFunc*)FramedSource::afterGetting, this);//��ʾ�ӳ�0�����ִ�� afterGetting ����
	return;
}

