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


//��Ϣ����
void ProcessMessages()
{
	MSG Message;
	if (::PeekMessage(&Message,NULL,0,0,PM_REMOVE)) 
	{
		::TranslateMessage(&Message);
		::DispatchMessage(&Message);
	}
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
	//����������(install, uninstall and etc.)
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


