#define  NULL 0
#ifndef _GLOBAL_PARAM_H
#define _GLOBAL_PARAM_H
#include <string>
#include <tool/MyMap.h>
class CGlobalParam
{
private:
	CGlobalParam();
	~CGlobalParam();
	static CGlobalParam* pGlobalParam;
	
public:
	static void Realease();
	static CGlobalParam* GetInstance();

	std::string m_HttpWorkDir;
	std::string m_SdkServerIp;
	int m_nSdkServerPort;
	int m_nSdkServerLine;

	std::string m_RtmpServerIp;
	int m_nRtmpkServerPort;

	bool m_bStopStream;//�Ƿ�ͣ��
	bool m_bStopStreamTimeOut;//ͣ����ʱʱ��


};
#endif