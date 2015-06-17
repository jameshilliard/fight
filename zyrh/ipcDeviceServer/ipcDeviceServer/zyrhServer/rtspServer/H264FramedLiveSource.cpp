#include "H264FramedLiveSource.hh" 
 

H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment& env,CdevSdk *ptCdevSdk, unsigned preferredFrameSize, unsigned playTimePerFrame)
: FramedSource(env)
{
	//Framed_datasize = datasize;//数据区大小指针
	//Framed_databuf = databuf;//数据区指针
	//Framed_dosent = dosent;//发送标示
	m_ptCdevSdk=ptCdevSdk;
	m_curVideoIndex=0;
}

void H264FramedLiveSource::setDeviceSource(CdevSdk *deviceSource)
{
	if(deviceSource==NULL)
		return;
	m_ptCdevSdk = deviceSource;
	m_ptCdevSdk->addDeviceSource(&m_h264Data);
	m_ptCdevSdk->ReStartDev();
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
	m_ptCdevSdk->removeDeviceSource(&m_h264Data);
}


void H264FramedLiveSource::doGetNextFrame()
{
	unsigned int dataSize=0;
	unsigned int frameSize=0;
	int i=0;
	bool bRet=true;
	if(m_ptCdevSdk==NULL)
		return;
	//while(bRet)
	{
		
		bRet=m_ptCdevSdk->GetVideoData(&m_h264Data,fTo+dataSize,frameSize,fMaxSize-dataSize,m_curVideoIndex);
		dataSize+=frameSize;
	}
	fFrameSize=dataSize;
	if(fFrameSize==0)
	{
		Sleep(100);
	}
	nextTask() = envir().taskScheduler().scheduleDelayedTask(0,(TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
	return;
}

