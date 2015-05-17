#pragma once 
#include "aac/faac.h"
#include "aac/faaccfg.h"
#include <boost/shared_ptr.hpp>
#include <x264/bytestream.h>
class CAacEncoder
{
public:
	CAacEncoder(int nSampleRate = 16000,int nNumChannels = 1);
	~CAacEncoder();
	void EncEncodeAac(char *data,int nlen,boost::shared_ptr<buffer> pAudioBuf);
private:
	faacEncHandle m_faacHandle;
	faacEncConfigurationPtr m_faacConfigure;
	int m_nSampleRate;//采样率
	int m_nNumChannels;//通道数
	unsigned long m_ninputSamples;
	unsigned long m_nmaxOutputBytes;
	char* m_outbuf;
	buffer m_Inbuf;
};