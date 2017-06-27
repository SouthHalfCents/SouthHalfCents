// BDHIDSService.h: interface for the CBDHIDSService class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _BDHIDSSERVICE_H_
#define _BDHIDSSERVICE_H_

//#if !defined(AFX_BDHIDSSERVICE_H__C76DC203_3B91_4373_B21A_812E79AC59F8__INCLUDED_)
//#define AFX_BDHIDSSERVICE_H__C76DC203_3B91_4373_B21A_812E79AC59F8__INCLUDED_
#include <winsvc.h>
//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000



#define SERVICE_CONTROL_USER 128
#define	BDHIDS_EVENT_NOTIFYAPPEXIT	 "Global\\AngoNotifyAppExitEvent"

#define BDHAUDIT_SVCNAME "AngoService"
#define BDHAUDIT_CLIENT "AngoTime.exe"


class CBDHIDSService  
{
public:
	void LogEvent(WORD wType, DWORD dwID,
		const char* pszS1 = NULL,
		const char* pszS2 = NULL,
		const char* pszS3 = NULL);
	BOOL Uninstall();
	BOOL StartService();
	void SetStatus(DWORD dwState);

	BOOL SendStatusToSCM(DWORD dwCurrentState, 
		DWORD dwWin32ExitCode, 
		DWORD dwServiceSpecificExitCode,
		DWORD dwCheckPoint, 
		DWORD dwWaitHint);
	static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	
	virtual void Run();
	BOOL ParseStandardArgs(int argc, char* argv[]);
	virtual BOOL OnUserControl(DWORD dwOpcode);
	virtual void OnStop();
	virtual void OnShutdown();
	virtual void OnPause();
	virtual void OnInterrogate();
	virtual BOOL OnInit();
	virtual void OnContinue();
	BOOL IsInstalled();
	BOOL Install();
	BOOL Initialize();
	static void WINAPI Handler(DWORD dwOpcode);
	static DWORD WINAPI HandlerEx(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext);

	
	static BOOL DoRegisterDeviceInterface(HANDLE hRecipient,HDEVNOTIFY *hDevNotify);


	BOOL DEBUGSET;
	BOOL m_bIsRunning;
	SERVICE_STATUS_HANDLE m_hServiceStatus;
	int m_iMinorVersion;
	int m_iMajorVersion;
	SERVICE_STATUS m_Status;
	char m_szServiceName[64];
	CBDHIDSService(const char* szServiceName);
	virtual ~CBDHIDSService();
	static CBDHIDSService* m_pThis;
protected:
	BOOL FindApp(const char * szAppName);
	BOOL FindMainApp(void);
	BOOL FindVncApp();
	BOOL FindVncSvcApp();

	BOOL FindDestopProc(HANDLE& hToken,DWORD& dwSessionId); 
	int BdGetWindowType(void);
	BOOL CreateDestopProcess(char * szPs);
	BOOL XP_CreateDestopProcess(char *szPs);
	BOOL Win7_CreateDestopProcess(char *szPs);


	bool IsUacEnabled(void);
	BOOL ExecCommand(CString csComm,BOOL bWait=TRUE);
	BOOL ExecCommand(CString csComm,WORD wShowWindow,BOOL bIsWaitFinish);
	BOOL StartSysService(char * szSrvName,BOOL bWait=TRUE);
	void BdHidsUpdate(void);

	BOOL StartClient(void);
	BOOL StopClient(void);

	BOOL ReadClientOptions(void);
	BOOL EnableVnc(void);
	BOOL StartRemoteMng(void);
	BOOL StartVncSvc(void);

	int m_nWinType;
	HANDLE m_hToken;
	DWORD m_dwSessonId;
	BOOL m_bHasStartClient,m_bHasLogout;
private:
	unsigned long m_SelectModule;
	unsigned long m_SecOptions;
	BOOL m_bEnableRemoteMng;
//    HANDLE m_hEventSource;
};

#endif // !defined(AFX_BDHIDSSERVICE_H__C76DC203_3B91_4373_B21A_812E79AC59F8__INCLUDED_)
