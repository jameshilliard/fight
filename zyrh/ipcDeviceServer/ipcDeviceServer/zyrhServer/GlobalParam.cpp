#include "GlobalParam.h"
#include <GlobalFun.h>
CGlobalParam* CGlobalParam::pGlobalParam = NULL;
CGlobalParam::CGlobalParam()
{
	m_HttpWorkDir = "D:/";
	CreatDir("D://live");

}
CGlobalParam::~CGlobalParam()
{
	 
}

CGlobalParam* CGlobalParam::GetInstance()
{
	if (pGlobalParam == NULL)
	{
		pGlobalParam = new CGlobalParam();
	}
	return pGlobalParam;
}
void CGlobalParam::Realease()
{
	if (pGlobalParam != NULL)
	{
		delete pGlobalParam;
		pGlobalParam = NULL;
	}
}
