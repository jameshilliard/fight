#ifndef _H264FRAMEDLIVESOURCE_HH
#define _H264FRAMEDLIVESOURCE_HH
#include "../devsdk/DevSdk.h"
#include <FramedSource.hh> 


class H264FramedLiveSource : public FramedSource
{
public:
	static H264FramedLiveSource* createNew(UsageEnvironment& env,CdevSdk *ptCdevSdk, unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);
	void setDeviceSource(CdevSdk *deviceSource)
	{
		m_ptCdevSdk = deviceSource;
		m_ptCdevSdk->addDeviceSource(&m_h264Data);
		m_ptCdevSdk->ReStartDev();
	}

protected:
	H264FramedLiveSource(UsageEnvironment& env,CdevSdk *ptCdevSdk, unsigned preferredFrameSize, unsigned playTimePerFrame);
	~H264FramedLiveSource();

private:
	virtual void doGetNextFrame();
	
	int TransportData(unsigned char* to, unsigned maxSize);
	
	
protected:
	unsigned int 				m_curVideoIndex;
	std::vector<std::string > 	m_h264Data;
	
public:
	CdevSdk 		*m_ptCdevSdk;

};

#endif
