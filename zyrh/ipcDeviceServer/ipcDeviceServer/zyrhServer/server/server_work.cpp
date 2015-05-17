#include "server_work.h"
#include <GlobalClass.h>
void server_work::SendData(handler_ptr handler,char* pBuf,unsigned int nBufLen)
{
	SendBufPtr sendBufPtr = CGlobalClass::GetInstance()->GetSendBuf();
	if (sendBufPtr)
	{
		sendBufPtr->InsertData((char*)pBuf,nBufLen);
		handler->async_send_normal(sendBufPtr);
	}

}