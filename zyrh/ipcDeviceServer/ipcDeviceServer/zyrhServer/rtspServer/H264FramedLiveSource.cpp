#include "H264FramedLiveSource.hh" 
 

H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment& env,CdevSdk *ptCdevSdk, unsigned preferredFrameSize, unsigned playTimePerFrame)
: FramedSource(env)
{
	//Framed_datasize = datasize;//��������Сָ��
	//Framed_databuf = databuf;//������ָ��
	//Framed_dosent = dosent;//���ͱ�ʾ
	m_ptCdevSdk=ptCdevSdk;
	m_lelfPackNums=0;
}

void H264FramedLiveSource::setDeviceSource(CdevSdk *deviceSource)
{
	if(deviceSource==NULL)
		return;
	m_ptCdevSdk = deviceSource;
	m_ptCdevSdk->ReStartDev();
	m_ptCdevSdk->GetKeyFrame();
	printf("start frame source is 0x%x---\n",this);
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
	int sleepTime=0;
	if(m_ptCdevSdk==NULL)
		return;
	bRet=m_ptCdevSdk->GetVideoData(fTo,fFrameSize,fMaxSize,fNumTruncatedBytes,m_lelfPackNums);
	sleepTime=25000;
	if(bRet==true && fFrameSize>0 && m_lelfPackNums>5)
	{
		sleepTime=0;
	}
	if(bRet==true && fFrameSize>0)
	{
		 gettimeofday(&fPresentationTime, NULL);//ͬһ֡��NAL������ͬ��ʱ���
	}
	fDurationInMicroseconds = sleepTime;//���Ʋ����ٶ�
	//gettimeofday(&fPresentationTime, NULL);
	//printf("fPresentationTime = %d.%d fDurationInMicroseconds=%d\n", fPresentationTime.tv_sec, fPresentationTime.tv_usec,fDurationInMicroseconds);      
	//afterGetting(this);  
	nextTask() = envir().taskScheduler().scheduleDelayedTask(sleepTime,(TaskFunc*)FramedSource::afterGetting, this);//��ʾ�ӳ�0�����ִ�� afterGetting ����
}

