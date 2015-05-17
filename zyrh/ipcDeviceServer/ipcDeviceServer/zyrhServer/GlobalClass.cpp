#include "GlobalClass.h"
CGlobalClass* CGlobalClass::pGlobal = NULL;
CGlobalClass* pGlobal1 = CGlobalClass::GetInstance();
#include "devsdk/DevManager.h"
CGlobalClass::CGlobalClass():m_Io_serviec_pool(5)
{
	init();
}
CGlobalClass::~CGlobalClass()
{
	uninit();
}
void CGlobalClass::init()
{
	g_pDevManager = new CDevManager;
	m_Io_serviec_pool.start();

}
void CGlobalClass::uninit()
{
	if (g_pDevManager != NULL)
	{
		delete g_pDevManager;
		g_pDevManager = NULL;
	}
	
}
CGlobalClass* CGlobalClass::GetInstance()
{
	if (pGlobal == NULL)
	{
		pGlobal = new CGlobalClass();
	}
	return pGlobal;
}
void CGlobalClass::Realease()
{
	if (pGlobal != NULL)
	{
		delete pGlobal;
		pGlobal = NULL;
	}
}

SendBufPtr CGlobalClass::GetSendBuf()
{
	return m_SendBufList.GetSendBuf();
}
void CGlobalClass::RealeaseSendBuf(SendBufPtr sendBuf)
{
	m_SendBufList.RealeaseSendBuf(sendBuf);
}

CDevManager*  CGlobalClass::GetDevManager()
{
	return g_pDevManager;
}
bas::io_service_pool* CGlobalClass::GetIoservice()
{
	return &m_Io_serviec_pool;
}