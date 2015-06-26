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

void H264FramedLiveSource::setDeviceSource(CdevSdk *deviceSource)
{
	if(deviceSource==NULL)
		return;
	m_ptCdevSdk = deviceSource;
	//m_ptCdevSdk->ReStartDev();
}

H264FramedLiveSource* H264FramedLiveSource::createNew(UsageEnvironment& env, CdevSdk *ptCdevSdk, unsigned preferredFrameSize, unsigned playTimePerFrame)
{
	//static H264FramedLiveSource  newSource(env, nCdevSdkParam, preferredFrameSize, playTimePerFrame);
	H264FramedLiveSource* newSource = new H264FramedLiveSource(env,ptCdevSdk, preferredFrameSize, playTimePerFrame);
	if(newSource)
		newSource->setDeviceSource(ptCdevSdk);
	return newSource;
}

H264FramedLiveSource::~H264FramedLiveSource()
{
	if(m_ptCdevSdk==NULL)
		return;
	//m_ptCdevSdk->StopPlay();
}


void H264FramedLiveSource::doGetNextFrame()
{
	bool bRet=true;
	if(m_ptCdevSdk==NULL)
		return;
	bRet=m_ptCdevSdk->GetVideoData(fTo,fFrameSize,fMaxSize,fNumTruncatedBytes,m_curVideoIndex);
	if(fFrameSize==0)
	{
		Sleep(20);
		bRet=m_ptCdevSdk->GetVideoData(fTo,fFrameSize,fMaxSize,fNumTruncatedBytes,m_curVideoIndex);
	}
	nextTask() = envir().taskScheduler().scheduleDelayedTask(0,(TaskFunc*)FramedSource::afterGetting, this);//��ʾ�ӳ�0�����ִ�� afterGetting ����
	return;
}

