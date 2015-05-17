#pragma once
#include <vector>
#include "TsEncoder.h"
#include "TimeRecalculate.h"
using namespace std;

struct TsFileBuf
{
	double duration;
	std::string fileName;
	unsigned int nTimeCreate;
	TsFileBuf()
	{
		nTimeCreate = time(NULL);
	}
};

class CM3u8List
{
public:
	CM3u8List();
	~CM3u8List(void);
	void handleVideo(uint8_t* buf,int size,__int64 nowTimeStamp,bool bkey);
	void handleAudio(uint8_t* buf,int size,__int64 nowTimeStamp);
	bool IsNeedCreateFile(__int64 nowTimeStamp,double& fileduration,bool bkey);
	std::string CreateM3u8File();
	std::string CreatTsName();
private:
	void ReSetParam();
	void UpdateM3u8List(TsFileBuf&);

	CTsEncoder* m_pTsCur;
	vector<TsFileBuf> m_vcTsList; 
	vector<TsFileBuf> m_vcTsDelList; 
	__int64 m_nLastTic;
	__int64 m_nBeginTic;
	__int64 m_nNowTic;
	__int64 m_nCreateTic;

	int   m_nTargetDuration;
	int   m_nSequence;
	boost::asio::detail::mutex mutex_Lock;
public:
	std::string m_sdveid;
	std::string m_sDir;
	CTimeRecalculate m_TimeRecal;
	
};
