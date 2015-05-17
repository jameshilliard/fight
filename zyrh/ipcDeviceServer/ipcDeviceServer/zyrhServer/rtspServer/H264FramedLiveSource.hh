#ifndef _H264FRAMEDLIVESOURCE_HH
#define _H264FRAMEDLIVESOURCE_HH
#include "../devsdk/H264FrameDeviceSource.h"
#include <FramedSource.hh> 


class H264FramedLiveSource : public FramedSource
{
public:
	static H264FramedLiveSource* createNew(UsageEnvironment& env,H264FrameDeviceSource *ptH264FrameDeviceSource, unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);
	void setDeviceSource(H264FrameDeviceSource *deviceSource)
	{
		m_ptH264FrameDeviceSource = deviceSource;
		m_ptH264FrameDeviceSource->addDeviceSource(&m_h264Data);
		m_ptH264FrameDeviceSource->ReStartDev();
	}

protected:
	H264FramedLiveSource(UsageEnvironment& env,H264FrameDeviceSource *ptH264FrameDeviceSource, unsigned preferredFrameSize, unsigned playTimePerFrame);
	~H264FramedLiveSource();

private:
	virtual void doGetNextFrame();
	
	int TransportData(unsigned char* to, unsigned maxSize);
	
	
protected:
	unsigned int 				m_curVideoIndex;
	std::vector<std::string > 	m_h264Data;
	
public:
	H264FrameDeviceSource 		*m_ptH264FrameDeviceSource;

};

#endif
