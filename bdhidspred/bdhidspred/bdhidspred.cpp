// bdhidspred.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <io.h>
#include <Shlwapi.h>
#include "bdhidspred.h"
#include "Sleeper.h"
#include "BDHIDSCoreService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

void SleeperFunc(DWORD dwMilliseconds)
{
	CSleeper Sleeper;
	Sleeper.Sleep(dwMilliseconds);
}

//判断文件否存在
/*
BOOL FileExists(char * szFile)
{
	if( (_access(szFile, 0 )) != -1 )
		return TRUE;
	return FALSE;
}
*/

BOOL FileExists(char * szFile)
{
	BOOL bRet=::PathFileExists(szFile);
	return bRet;
}
//消息处理
void ProcessMessages()
{
	MSG Message;
	if (::PeekMessage(&Message,NULL,0,0,PM_REMOVE)) 
	{
		::TranslateMessage(&Message);
		::DispatchMessage(&Message);
	}
}

void DebugView(const char* pszFormat, ...)
{
#ifdef _DEBUG
	char buf[8192];
	char date[50];
	char time[50];

	SYSTEMTIME st;

	GetLocalTime(&st);
	GetDateFormat(LOCALE_SYSTEM_DEFAULT,0,&st,"yyyy'-'MM'-'dd",date,sizeof(date));
	GetTimeFormat(LOCALE_SYSTEM_DEFAULT,0,&st,"HH':'mm':'ss",time,sizeof(time));

	_snprintf(buf, 8192, "[BDAuthSys](%s %s):",date,time);
	//_snprintf(buf, 8192, "[BDAServer](%lu - %s %s): ", GetCurrentThreadId(),date,time);
	va_list arglist;
	va_start(arglist, pszFormat);
	_vsnprintf(&buf[strlen(buf)],8192-strlen(buf)-1,pszFormat,arglist);
	va_end(arglist);
	_snprintf(buf,8192,"%s\n",buf);
	OutputDebugString(buf);

	FILE *fp=fopen("c:\\Ango.txt","a+");
	if(fp!=NULL)
	{
		fprintf(fp,buf);
		fclose(fp);
	}
#endif

}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		//cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}

	CBDHIDSCoreService CustomServiceObject;
	CString strtmp;

#ifdef _DEBUGVIEW   
	OutputDebugString("Entering bdhidspred::tmain()\n");
#endif
	//处理命令行(install, uninstall and etc.)
	if (!CustomServiceObject.ParseStandardArgs(argc, argv)) 
	{
		if(argc>1) 
		{

			CustomServiceObject.DEBUGSET=1;

			if (CustomServiceObject.OnInit())
			{
				CustomServiceObject.m_bIsRunning = TRUE;
				CustomServiceObject.Run();
			}

		} 
		else 
		{
			CustomServiceObject.StartService();
		}
	}

	// When we get here, the service has been stopped
	return CustomServiceObject.m_Status.dwWin32ExitCode;
	//return nRetCode;
}


