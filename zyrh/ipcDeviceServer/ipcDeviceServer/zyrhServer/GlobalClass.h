#define  NULL 0
#ifndef _GLOBAL_CLASS_H
#define _GLOBAL_CLASS_H
#include <bas/Sendbuffer.hpp>
#include <bas/io_service_pool.hpp>
class CDevManager;
class CGlobalClass
{
private:
	CGlobalClass();
	~CGlobalClass();
	static CGlobalClass* pGlobal;
	void uninit();
	void init();
public:
	static CGlobalClass* GetInstance();
	static void Realease();

	SendBufPtr GetSendBuf();
	void RealeaseSendBuf(SendBufPtr sendBuf);

	CDevManager* GetDevManager();
	bas::io_service_pool* GetIoservice();

private:
	CSendBufList   m_SendBufList;
public:
	CDevManager*    g_pDevManager;
	bas::io_service_pool m_Io_serviec_pool;
	


};
#endif