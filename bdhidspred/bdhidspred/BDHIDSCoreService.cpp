// BDHIDSCoreService.cpp: implementation of the CBDHIDSCoreService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Psapi.h>
#include <windows.h>
#include <tlhelp32.h>
#include "bdhidspred.h"
#include "CRegistry.h"
#include "BDHIDSCoreService.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Duplicate of the CNTService DebugMsg
void DebugMsg(const char* pszFormat, ...)
{
#ifdef _DEBUGVIEW   
	char buf[8192];
	char date[50];
	char time[50];

	SYSTEMTIME st;

	GetLocalTime(&st);
	GetDateFormat(LOCALE_SYSTEM_DEFAULT,0,&st,"dd'/'MM'/'yyyy",date,sizeof(date));
	GetTimeFormat(LOCALE_SYSTEM_DEFAULT,0,&st,"HH':'mm':'ss",time,sizeof(time));

	_snprintf(buf, 8192, "[SNARE](%lu - %s %s): ", GetCurrentThreadId(),date,time);
	va_list arglist;
	va_start(arglist, pszFormat);
	_vsnprintf(&buf[strlen(buf)],8192-strlen(buf)-1,pszFormat,arglist);
	va_end(arglist);
	_snprintf(buf,8192,"%s\n",buf);
	OutputDebugString(buf);
#endif
}

CBDHIDSCoreService::CBDHIDSCoreService():CBDHIDSService(BDHAUDIT_SVCNAME)
{
	m_dwThreadID=0L;
}

CBDHIDSCoreService::~CBDHIDSCoreService()
{

}

BOOL CBDHIDSCoreService::OnInit()
{
	return TRUE;
}

void CBDHIDSCoreService::OnShutdown()
{
	DebugView("CBDHIDSCoreService::OnShutdown()");
	//system("")
	//StopClient();
}

BOOL CBDHIDSCoreService::OnUserControl(DWORD dwOpcode)
{
	switch (dwOpcode) 
	{
	case SERVICE_CONTROL_USER + 0:
		// Save the current status in the registry
		return TRUE;

	default:
		break;
	}
	return FALSE; // say not handled
}


void CBDHIDSCoreService::Run()
{
	BOOL bRet=FALSE;
	unsigned int nDelectCount=0;

	unsigned int dwStartTime,dwStopTime;
	dwStartTime=::GetTickCount();

	m_dwThreadID = GetCurrentThreadId();
	BdHidsUpdate();
	//bRet=StartSysService("BDHIDSD");
	m_nWinType=BdGetWindowType();
	ReadClientOptions();



	SECURITY_ATTRIBUTES		sa;
	SECURITY_DESCRIPTOR		sd;

	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;
	m_bHasStartClient=FALSE;


	//MSG msg;
	while(m_bIsRunning)
	{
		//DebugView("CBDHIDSCoreService is running...");
		::Sleep(1000);
		if(m_nWinType==2)//win2k
		{
			dwStopTime=::GetTickCount();
			if(dwStopTime<dwStartTime)
			{
				dwStartTime=dwStopTime;
			}
			else if((dwStopTime-dwStartTime)>20000)
			{
				dwStartTime=dwStopTime;
				if(!FindMainApp())
				{
					m_bHasStartClient=FALSE;
				}
			}
		}
		if(!m_bHasStartClient)
		{
			DebugView("CBDHIDSCoreService::Run()  m_nWinType:%d",m_nWinType);
			if(m_nWinType>=5)
			{
				if(FindDestopProc(m_hToken,m_dwSessonId)) //服务器端不一定有用户登录
				{
					bRet=StartClient();
					m_bHasStartClient=TRUE;
					nDelectCount=0;
				}
			}
			else
			{
				bRet=StartClient();
				m_bHasStartClient=TRUE;
				nDelectCount=0;
			}

		}
		else
		{
			nDelectCount++;
			if(nDelectCount>=20)
			{
				if(!FindMainApp())
				{
					m_bHasStartClient=FALSE;
				}
				nDelectCount=0;
			}
		}

	

		/**
		if(GetMessage(&msg, 0, 0, 0))
		{
		DispatchMessage(&msg);
		}
		**/
	}



	/*****
	while(m_bIsRunning) 
	{

	MSG msg;

	while (PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
	{ 

	if(!m_bIsRunning)
	{
	//OutputDebugString("notity exit ok!\n");
	break;
	}

	} 


	if(!m_bIsRunning) break;  


	SleeperFunc(1000);
	//OutputDebugString("CBDHIDSCoreService::Run() Runing...\n");
	}
	***/

	// nothing more to do
	DebugMsg("Leaving CBDHIDSCoreService::Run()");

}

void CBDHIDSCoreService::OnStop()
{
	m_bIsRunning = FALSE;
	DebugMsg("BDHIDSPRED ONSTOP");
	SetStatus(SERVICE_STOP_PENDING);
	PostThreadMessage(m_dwThreadID, WM_QUIT, 0, 0);
}



