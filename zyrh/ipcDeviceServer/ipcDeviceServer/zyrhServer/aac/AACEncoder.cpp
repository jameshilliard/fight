#include "stdafx.h"
#include "AACEncoder.h"
#include <mmsystem.h>  
#pragma comment(lib, "winmm.lib") 




CAacEncoder::CAacEncoder(int nSampleRate,int nNumChannels):m_Inbuf(1024*20)
{
	m_nNumChannels = nNumChannels;
	m_nSampleRate = nSampleRate;
	m_faacHandle = faacEncOpen(nSampleRate,nNumChannels,&m_ninputSamples,&m_nmaxOutputBytes);
	faacEncConfigurationPtr aacConfigure = faacEncGetCurrentConfiguration(m_faacHandle);
	aacConfigure->inputFormat= FAAC_INPUT_16BIT;
	aacConfigure->mpegVersion = MPEG4;
	aacConfigure->aacObjectType = LOW;
	aacConfigure->allowMidside = 1;
	aacConfigure->useLfe = 0;
	aacConfigure->useTns = 0;
	//aacConfigure->bitRate = 16000;
	aacConfigure->quantqual = 100;
	aacConfigure->bandWidth = 0;
	aacConfigure->outputFormat = 1;
	faacEncSetConfiguration(m_faacHandle, aacConfigure);
	m_outbuf = new char[m_nmaxOutputBytes];
}
CAacEncoder::~CAacEncoder()
{
	delete[] m_outbuf;
	faacEncClose(m_faacHandle);
}
void CAacEncoder::EncEncodeAac(char *data,int nlen,boost::shared_ptr<buffer> pAudioBuf)
{
	int ret = 0;
 	append_data((buffer*)&m_Inbuf,(unsigned char *)data,nlen);
	if (m_Inbuf.d_cur<m_ninputSamples*2)
	{
		return;
	}
	ret = faacEncEncode(m_faacHandle, (int*)m_Inbuf.data, m_ninputSamples, (unsigned char *)m_outbuf, m_nmaxOutputBytes);
	if (ret > 0)
	{
		append_data(pAudioBuf.get(),(unsigned char *)m_outbuf,ret);	
	}
	move(&m_Inbuf,m_ninputSamples*2);
}
