// BDHIDSService.cpp: implementation of the CBDHIDSService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>

#include <DBT.h>
#include <devguid.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <lm.h>
#include <Ntsecapi.h>
#include <Accctrl.h>
#include <Aclapi.h>
#include <tchar.h>

#include "bdhidspred.h"
#include "bdhidsservice.h"
#include "..\..\public\common\Utils.h"

#include <Wtsapi32.h>
#include <Userenv.h>
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "advapi32.lib")
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
int g_WinType=0;
CBDHIDSService* CBDHIDSService::m_pThis = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDHIDSService::CBDHIDSService(const char* szServiceName)
{
	m_pThis = this;

	m_SelectModule=0;
	m_SecOptions=0;
	m_bEnableRemoteMng=FALSE;

	m_hToken=NULL;
	m_dwSessonId=0L;
	m_nWinType=0;
	m_bHasStartClient=FALSE;
	m_bHasLogout=FALSE;

	strncpy(m_szServiceName, szServiceName, sizeof(m_szServiceName)-1);
	m_iMajorVersion = 1;
	m_iMinorVersion = 0;
	//    m_hEventSource = NULL;

	m_hServiceStatus = NULL;
	m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_Status.dwCurrentState = SERVICE_STOPPED;
	m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN; 
	m_Status.dwWin32ExitCode = 0;
	m_Status.dwServiceSpecificExitCode = 0;
	m_Status.dwCheckPoint = 0;
	m_Status.dwWaitHint = 0;
	m_bIsRunning = FALSE;
}

CBDHIDSService::~CBDHIDSService()
{

}


BOOL CBDHIDSService::FindApp(const char * szAppName)
{
	BOOL bIsFind=FALSE;

	if(szAppName==NULL)
		return FALSE;
	//notice:only use in win2000/winxp/win2003
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (!hSnapshot)
	{
		return FALSE;
	}

	// prepare to enumerate the processes....
	PROCESSENTRY32 pe;

	// remember to init the structure...
	pe.dwSize=sizeof(PROCESSENTRY32);
	BOOL retval=Process32First(hSnapshot,&pe);
	while(retval)
	{
		::ProcessMessages();

		if(!_stricmp(pe.szExeFile,szAppName))
		{
			bIsFind=TRUE;
			break;
		} 
		retval=Process32Next(hSnapshot,&pe);
	}

	// since we are done, remember to close the snapshot handle
	CloseHandle(hSnapshot);

	return bIsFind;
}

BOOL CBDHIDSService::FindMainApp()
{
	BOOL bIsFind=FindApp(BDHAUDIT_CLIENT);

	return bIsFind;
}

BOOL CBDHIDSService::FindVncApp()
{
	BOOL bIsFind=FindApp("BDWinVNC.exe");

	return bIsFind;
}

BOOL CBDHIDSService::FindVncSvcApp()
{
	BOOL bIsFind=FindApp("bdvncsvr.exe");

	return bIsFind;
}


HANDLE GetToken(DWORD dwProcessID) 
{
HANDLE hProcess	  = NULL;
HANDLE hToken     = NULL;
HANDLE g_hToken   = NULL;

PSECURITY_DESCRIPTOR pSD = NULL;

    HANDLE hCurToken=NULL;
	LUID DebugNameValue;
	TOKEN_PRIVILEGES Privileges;
	DWORD dwRet;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hCurToken);
	LookupPrivilegeValue(NULL, "SeDebugPrivilege", &DebugNameValue);
	Privileges.PrivilegeCount=1;
	Privileges.Privileges[0].Luid=DebugNameValue;
	Privileges.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hCurToken, FALSE,&Privileges, sizeof(Privileges), NULL, &dwRet);
	CloseHandle(hCurToken);
try {	
// Get the handle to the process
hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID);
if (hProcess == NULL) 	 
{
	DWORD dwErr=::GetLastError();
	return NULL;
}

HANDLE hNewToken = NULL;

OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken);

//CloseHandle(hProcess);






//DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hNewToken);

//CloseHandle(hToken);

/*
// Get the token (All access so we can change and launch things
if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken)) 
{
CloseHandle(hProcess);
return NULL;
}
*/


// Get memory for an SD
pSD = (PSECURITY_DESCRIPTOR) GlobalAlloc(GPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
if (pSD == NULL) 
{
CloseHandle(hNewToken);
CloseHandle(hProcess);
return NULL;
}

// Initialize it
if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) 
{
GlobalFree(pSD);
CloseHandle(hNewToken);
CloseHandle(hProcess);
return NULL;
}

// Add a NULL DACL to the security descriptor..
if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE)) 
{
GlobalFree(pSD);
CloseHandle(hNewToken);
CloseHandle(hProcess);
return NULL;
}

// We made the security descriptor just in case they want a duplicate.
// We make the duplicate have all access to everyone.
SECURITY_ATTRIBUTES sa;
sa.nLength              = sizeof(sa);
sa.lpSecurityDescriptor = pSD;
sa.bInheritHandle       = TRUE;

// If the user chooses not to copy the token, then changes made to it
// will effect the owning process
//        if (IDNO == ::MessageBox(NULL, TEXT("Would you like to make a copy of ")
//            TEXT("this process token?\n(Selecting \"No\" will cause the ")
//            TEXT("\"AdjustToken\" and \"SetToken\"\nfeatures to affect the ")
//            TEXT("owning process.) "), TEXT("Duplicate Token?"), MB_YESNO)) 

// Duplicate the token
if (!DuplicateTokenEx(hToken,MAXIMUM_ALLOWED/* TOKEN_ALL_ACCESS*/, &sa,
SecurityImpersonation, TokenPrimary, &g_hToken)) 
{
CloseHandle(hToken);
//CloseHandle(hProcess);
GlobalFree(pSD);
return NULL;
}
} 
catch(...) 
{}

//CloseHandle(hToken);
//CloseHandle(hProcess);
GlobalFree(pSD);
return g_hToken;
}



BOOL CBDHIDSService::FindDestopProc(HANDLE& hToken,DWORD& dwSessionId)
{

	//DebugBreak();
	//DebugView("Enter CBDHIDSCoreService::FindDestopProc()...");
	BOOL bIsFind=FALSE;
	//notice:only use in win2000/winxp/win2003
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (!hSnapshot)
	{
		CUtils::DebugShow("Leave CBDHIDSCoreService::CreateToolhelp32Snapshot failed");
		return FALSE;
	}


	// prepare to enumerate the processes....
	PROCESSENTRY32 pe;

	// remember to init the structure...
	pe.dwSize=sizeof(PROCESSENTRY32);
	BOOL retval=Process32First(hSnapshot,&pe);
	while(retval && m_bIsRunning)
	{
		::ProcessMessages();

		if(!_stricmp(pe.szExeFile,/*"winlogon.exe"*/  "explorer.exe"  /*"bdhidspred.exe"*/))
		{
			CUtils::DebugShow("IN CBDHIDSCoreService::FindDestopProc=> Process id:%ld  name:%s\n",pe.th32ProcessID,pe.szExeFile);
			dwSessionId=0L;

			ProcessIdToSessionId(pe.th32ProcessID, &dwSessionId); 
			
			HANDLE hCurToken=NULL;
			LUID DebugNameValue;
			TOKEN_PRIVILEGES Privileges;
			DWORD dwRet;
			OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hCurToken);
			LookupPrivilegeValue(NULL, "SeDebugPrivilege", &DebugNameValue);
			Privileges.PrivilegeCount=1;
			Privileges.Privileges[0].Luid=DebugNameValue;
			Privileges.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hCurToken, FALSE,&Privileges, sizeof(Privileges), NULL, &dwRet);
			CloseHandle(hCurToken);

			
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
				FALSE,pe.th32ProcessID);
			BOOL bRet = OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken); 
			
			//hToken=GetToken(pe.th32ProcessID);
 			bIsFind=TRUE;
			break;
		} 
		retval=Process32Next(hSnapshot,&pe);
	}

	// since we are done, remember to close the snapshot handle
	CloseHandle(hSnapshot);
	//DebugView("Leave CBDHIDSCoreService::FindDestopProc()...");


	return bIsFind;
}


BOOL CBDHIDSService::ExecCommand(CString csComm,BOOL bWait)
{
	//STARTUPINFO s;
	STARTUPINFO si = { sizeof(si) };
	GetStartupInfo(&si);

	//GetStartupInfo(&s);
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags =STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES; 
	si.wShowWindow =SW_HIDE;

	PROCESS_INFORMATION pi;
	memset(&pi,0x00,sizeof(PROCESS_INFORMATION));

	if(CreateProcess(NULL,csComm.GetBuffer(csComm.GetLength()),NULL,NULL,FALSE,NULL,NULL,NULL,&si,&pi))
	{ 
		if(bWait)
		{
			WaitForSingleObject(pi.hProcess ,INFINITE); 
			CloseHandle(pi.hProcess ); 
			CloseHandle(pi.hThread );
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CBDHIDSService::ExecCommand(CString csComm,WORD wShowWindow,BOOL bIsWaitFinish)
{
	STARTUPINFO s;

	GetStartupInfo(&s);
	s.cb = sizeof(s);
	s.dwFlags =STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES; 
	s.wShowWindow =wShowWindow;

	PROCESS_INFORMATION pi;
	memset(&pi,0x00,sizeof(PROCESS_INFORMATION));
	//AfxMessageBox(csComm);

	if(CreateProcess(NULL,(LPSTR)(LPCSTR)csComm,NULL,NULL,FALSE,NULL,NULL,NULL,&s,&pi))
	{ 
		if(bIsWaitFinish)
		{
			DWORD dwWait=WaitForSingleObject(pi.hProcess ,INFINITE); 
			CloseHandle(pi.hProcess ); 
			CloseHandle(pi.hThread ); 
			return(dwWait==WAIT_OBJECT_0);
		}
		//AfxMessageBox("ok");
	}
	else
	{
		//AfxMessageBox("failure");
		return FALSE;
	}
	return TRUE;
}


BOOL CBDHIDSService::StartSysService(char * szSrvName,BOOL bWait)
{
	BOOL retval = FALSE;

	SC_HANDLE hSC = ::OpenSCManager( NULL, 
		NULL, GENERIC_EXECUTE);
	if( hSC == NULL)
	{
		return retval;
	}
	// 打开服务。
	SC_HANDLE hSvc = ::OpenService( hSC, szSrvName,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if( hSvc == NULL)
	{
		//        TRACE( "Open www erron。");
		::CloseServiceHandle( hSC);
		return retval;
	}
	// 获得服务的状态
	SERVICE_STATUS status;
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)
	{
		//      TRACE( "Get Service state error。");
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return retval;
	}
	//如果处于停止状态则启动服务，否则停止服务。

	if( status.dwCurrentState == SERVICE_RUNNING)
	{
		return TRUE;
	}
	else if( status.dwCurrentState == SERVICE_STOPPED)
	{
		retval = TRUE;
	}
	if(!retval) return retval;

	retval=FALSE;
	if( ::StartService( hSvc, NULL, NULL) == FALSE)
	{
		TRACE( "start service error。");
		::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
		return FALSE;
	}

	if(bWait)
	{
		// 等待服务启动
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)
		{
			::Sleep( status.dwWaitHint);
			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				//AfxMessageBox( "start success。");
				::CloseServiceHandle( hSvc);
				::CloseServiceHandle( hSC);
				retval=TRUE;
				break;
			}

		}
	}

	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);
	return retval;
}

void CBDHIDSService::BdHidsUpdate(void)
{
	char szAppPath[MAX_PATH];
	::GetModuleFileName(NULL,szAppPath,MAX_PATH);
	char * pStr=strrchr(szAppPath,'\\');
	if(pStr!=NULL) 
		*pStr=0x00;

	char szFile[MAX_PATH];
	_snprintf(szFile,MAX_PATH,"%s\\bdupdate.exe",szAppPath);
	if(CUtils::FileExist(szFile))
	{
		BOOL bRet=ExecCommand(szFile);
		::SleeperFunc(50);
		::DeleteFile(szFile);
	}

	_snprintf(szFile,MAX_PATH,"%s\\bdhidsupdate.exe",szAppPath);
	if(CUtils::FileExist(szFile))
	{
		BOOL bRet=ExecCommand(szFile);
		::SleeperFunc(50);
		::DeleteFile(szFile);
	}
}




BOOL CBDHIDSService::ReadClientOptions(void)
{

	char szFile[MAX_PATH],szAppPath[MAX_PATH];
	::GetModuleFileName(NULL,szAppPath,MAX_PATH);
	char * pStr=strrchr(szAppPath,'\\');
	if(pStr!=NULL) 
		*pStr=0x00;


	_snprintf(szFile,MAX_PATH,"%s\\extoptions.ini",szAppPath);


	UINT nValue=GetPrivateProfileInt("OPTIONS","EnableRemoteMng",1,szFile); 
	if(nValue>0)
		m_bEnableRemoteMng=TRUE;
	else
		m_bEnableRemoteMng=FALSE;

	return TRUE;
}

BOOL CBDHIDSService::EnableVnc(void)
{
	return m_bEnableRemoteMng;
}

int CBDHIDSService::BdGetWindowType()
{
	int nWinType = 0;//0:windows 9x 1:windows NT  2: windows 2000  3: windows xp 4:windows 2003 server
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;


	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{

		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return  0;
	}

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:

		if ( osvi.dwMajorVersion <= 4 )
		{
			nWinType = 1;
			return nWinType;
		}

		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			nWinType = 2;
			return nWinType;
		}

		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			nWinType = 3;
			return nWinType;
		}

		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			nWinType = 4;
			return nWinType;
		}

		else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
		{
			nWinType = 5;
			return nWinType;
		}

		else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 )
		{
			nWinType = 6;
			return nWinType;
		}
		else if ( osvi.dwMajorVersion == 6 && (osvi.dwMinorVersion == 2 || osvi.dwMinorVersion == 3))
		{
			nWinType = 7;
			return nWinType;
		}
		break;
	default:
		break;
	}

	return nWinType;

}

bool CBDHIDSService::IsUacEnabled(void)
{
	LPCSTR pszSubKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
	LPCSTR pszValue = "EnableLUA";
	DWORD dwType = REG_DWORD;
	DWORD dwValue = 0;
	DWORD dwValueSize=sizeof(DWORD);
	HKEY hKEY;
	LONG lRet=::RegOpenKeyEx(HKEY_LOCAL_MACHINE,pszSubKey,0,KEY_READ,&hKEY);
	if(lRet!=ERROR_SUCCESS)
	{
		DWORD dwLastError=::GetLastError();

		return true;
	}
	if (ERROR_SUCCESS!=::RegQueryValueExA(hKEY,pszValue,NULL,&dwType,(LPBYTE)&dwValue,&dwValueSize))
	{
		RegCloseKey(hKEY);
		return true;
	}
	if (dwValue>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

LUID GetLuidFromText(char *s)
{ 
	LUID Luid;

	Luid.LowPart = 0;
	Luid.HighPart = 0;
	if(!LookupPrivilegeValue(NULL,s,&Luid))
	{ 
		printf("LookupPrivilegeValue under GetLuidFromText(\"%s\") Failed:%d\n",s,GetLastError()); 
		return Luid;
	}
	return Luid;
}


//建立Administrators和SYSTEM 共用的privilege
TOKEN_PRIVILEGES * MakeAdminPriv(void)
{ 
	TOKEN_PRIVILEGES *token_privileges;
	DWORD i,PrivilegeCount;

	i = 0;
	PrivilegeCount = 24;
	token_privileges = (PTOKEN_PRIVILEGES) malloc(4 + sizeof(LUID_AND_ATTRIBUTES)*PrivilegeCount + 4);
	if(token_privileges == NULL)
	{ 
		printf("malloc failed for PTOKEN_PRIVILEGES in NtCreateTokenAsuser\n");
		return NULL;
	}
	token_privileges->PrivilegeCount = PrivilegeCount;

	//0
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeTcbPrivilege");
	//1
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeCreateTokenPrivilege");
	//2
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeTakeOwnershipPrivilege");
	//3 
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeCreatePagefilePrivilege");
	//4
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeLockMemoryPrivilege");
	//5
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeAssignPrimaryTokenPrivilege");
	//6
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeIncreaseQuotaPrivilege");
	//7 
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeIncreaseBasePriorityPrivilege");
	//8
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeCreatePermanentPrivilege");
	//9
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeDebugPrivilege");
	//10
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeAuditPrivilege");
	//11
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeSecurityPrivilege");
	//12
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeSystemEnvironmentPrivilege");
	//13
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeChangeNotifyPrivilege");
	//14
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeBackupPrivilege");
	//15
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeRestorePrivilege");
	//16
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeShutdownPrivilege");
	//17
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeLoadDriverPrivilege");
	//18
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeProfileSingleProcessPrivilege");
	//19
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeSystemtimePrivilege");
	//20
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeUndockPrivilege");
	//21
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeManageVolumePrivilege");
	//22
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeImpersonatePrivilege");
	//23
	token_privileges->Privileges[i].Attributes = 3;
	token_privileges->Privileges[i++].Luid = GetLuidFromText("SeCreateGlobalPrivilege");

	return token_privileges;
}



BOOL CBDHIDSService::CreateDestopProcess(char * szPs)
{

	OSVERSIONINFOEX os; 
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   
	if( !GetVersionEx((OSVERSIONINFO *)&os)) 
	{
		return FALSE;
	}

	CUtils::DebugShow("系统版本号: %d.%d", os.dwMajorVersion, os.dwMinorVersion);
	if(os.dwMajorVersion >= 6)
	{
		return Win7_CreateDestopProcess(szPs);
	}
	else
	{
		return XP_CreateDestopProcess(szPs);
	}








	/*
	HWND deskton;
	DWORD desktonID;
	HANDLE hProc;

	//deskton = FindWindow("Progman", "Program Manager");//有时候Findwindow这个函数执行不成功
	deskton = GetDesktopWindow();
	GetWindowThreadProcessId(deskton,&desktonID); 
	hProc = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,desktonID);
	if(hProc)
	{
		DebugView("OpenProcess fail ");
		return FALSE;
	}

	if ( !OpenProcessToken(hProc,TOKEN_DUPLICATE,&hToken) )
	{

	}




	HANDLE hUserTokenDup=NULL;
	//TOKEN_PRIVILEGES *pPrivileges=MakeAdminPriv();	


    
	DuplicateTokenEx(m_hToken,MAXIMUM_ALLOWED,NULL,
		SecurityIdentification,TokenPrimary,&hUserTokenDup);
	int dup = GetLastError();
	DebugView("DuplicateTokenEx dwLastError=%lu",dup);

	hUserTokenDup=m_hToken;
	//Adjust Token privilege

	
	BOOL bRet=SetTokenInformation(hUserTokenDup,
		TokenSessionId,(void*)m_dwSessonId,sizeof(DWORD));




	WCHAR wszFile[MAX_PATH]={0};
	::MultiByteToWideChar(CP_ACP,0,szPs,strlen(szPs),wszFile,sizeof(wszFile)/sizeof(wszFile[0]));

	DWORD   dwLogonFlags=LOGON_WITH_PROFILE;//LOGON_NETCREDENTIALS_ONLY;//LOGON_NETCREDENTIALS_ONLY/   
	DWORD   dwCreationFlags   =   CREATE_NEW_CONSOLE   |   CREATE_NEW_PROCESS_GROUP;   
	STARTUPINFOW   si2;   
	ZeroMemory(&si2,   sizeof(STARTUPINFOW));   
	si2.cb                 =   sizeof(STARTUPINFOW);   
	si2.lpDesktop   =   L"winsta0\\default";   
	si2.dwFlags =STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES; 
	si2.wShowWindow =SW_SHOWNORMAL;//SW_HIDE
	PROCESS_INFORMATION   pi2;   
	ZeroMemory(&pi2,sizeof(PROCESS_INFORMATION));


	bRet=CreateProcessAsUserW(hUserTokenDup,NULL,wszFile,NULL,NULL,TRUE,dwCreationFlags,NULL,NULL,&si2,&pi2);
	DWORD dwLastError=::GetLastError();
	DebugView("CreateProcessAsUserW bRet=%d dwLastError=%lu",bRet,dwLastError);
	if(bRet)   
	{   
		CloseHandle(pi2.hThread);   
		CloseHandle(pi2.hProcess);   
	}   

	if(hUserTokenDup!=NULL)
		::CloseHandle(hUserTokenDup);

	return bRet; 
	*/
}


BOOL CBDHIDSService::Win7_CreateDestopProcess(char *szPs)
{
	//http://blog.csdn.net/sonsie007/article/details/8835830#

	BOOL bReturn = FALSE;

	DWORD dwRet = 0;  
	PROCESS_INFORMATION pi;  
	STARTUPINFO si;  
	DWORD dwSessionId;//当前会话的ID  
	HANDLE hUserToken = NULL;//当前登录用户的令牌  
	HANDLE hUserTokenDup = NULL;//复制的用户令牌  
	HANDLE hPToken = NULL;//进程令牌  
	DWORD dwCreationFlags;  

	//得到当前活动的会话ID，即登录用户的会话ID  
	dwSessionId = WTSGetActiveConsoleSessionId();  
	CUtils::DebugShow("sessionid: %d",dwSessionId);
	do   
	{  
		WTSQueryUserToken(dwSessionId,&hUserToken);//读取当前登录用户的令牌信息  
		dwCreationFlags = NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE;//创建参数  

		ZeroMemory(&si,sizeof(STARTUPINFO));  
		ZeroMemory(&pi,sizeof(pi));  

		si.cb = sizeof(STARTUPINFO);  
		si.lpDesktop = "winsta0\\default";//指定创建进程的窗口站，Windows下唯一可交互的窗口站就是WinSta0\Default  

		TOKEN_PRIVILEGES tp;  
		LUID luid;  

		//打开进程令牌  
		if (!OpenProcessToken(GetCurrentProcess(),  
			TOKEN_ADJUST_PRIVILEGES|  
			TOKEN_QUERY|  
			TOKEN_DUPLICATE|  
			TOKEN_ASSIGN_PRIMARY|  
			TOKEN_ADJUST_SESSIONID|  
			TOKEN_READ|  
			TOKEN_WRITE,&hPToken))  
		{  
			dwRet = GetLastError();  
			break;  
		}  

		//查找DEBUG权限的UID  
		if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid))  
		{  
			CUtils::DebugShow( "LookupPrivilegeValue:"+CUtils::GetLastErrorStr());
			break;  
		}  

		//设置令牌信息  
		tp.PrivilegeCount = 1;  
		tp.Privileges[0].Luid = luid;  
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  

		//复制当前用户的令牌  
		if (!DuplicateTokenEx(hPToken,MAXIMUM_ALLOWED,NULL,SecurityIdentification,  
			TokenPrimary,&hUserTokenDup))  
		{  
			CUtils::DebugShow( "DuplicateTokenEx:"+CUtils::GetLastErrorStr());
			break;  
		}  

		//设置当前进程的令牌信息  
		if (!SetTokenInformation(hUserTokenDup,TokenSessionId,(void*)&dwSessionId,sizeof(DWORD)))  
		{  
			CUtils::DebugShow( "SetTokenInformation:"+CUtils::GetLastErrorStr());
			break;  
		}  

		//应用令牌权限  
		if (!AdjustTokenPrivileges(hUserTokenDup,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),  
			(PTOKEN_PRIVILEGES)NULL,NULL))  
		{  
			CUtils::DebugShow( "AdjustTokenPrivileges:"+CUtils::GetLastErrorStr());
			break;  
		}  

		//创建进程环境块，保证环境块是在用户桌面的环境下  
		LPVOID pEnv = NULL;  
		if (CreateEnvironmentBlock(&pEnv,hUserTokenDup,TRUE))  
		{  
			dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;  
		}  
		else  
		{  
			pEnv = NULL;  
		}  

		//创建用户进程  
		if (!CreateProcessAsUser(hUserTokenDup,NULL,szPs,NULL,NULL,FALSE,  
			dwCreationFlags,pEnv,NULL,&si,&pi))  
		{  
			CUtils::DebugShow( "CreateProcessAsUser:"+CUtils::GetLastErrorStr());
			break;  
		} 

		bReturn = TRUE;
	} while (0);  

	//关闭句柄  
	if (NULL != hUserToken)  
	{  
		CloseHandle(hUserToken);  
	}  

	if (NULL != hUserTokenDup)  
	{  
		CloseHandle(hUserTokenDup);  
	}  

	if (NULL != hPToken)  
	{  
		CloseHandle(hPToken);  
	}  

	return bReturn;  
}

BOOL CBDHIDSService::XP_CreateDestopProcess(char *szPs)
{


	HINSTANCE hInstance = ShellExecute(NULL, _T("open"), szPs, NULL, NULL, SW_SHOWNORMAL);
	if((int)hInstance > 32)
	{
		return TRUE;
	}
	else
	{
		CUtils::DebugShow("ShellExecute fail error : %d", (int)hInstance);
		return FALSE;
	}
}

BOOL CBDHIDSService::StartClient(void)
{
	BOOL bRet=FALSE;
	char szAppPath[MAX_PATH];
	::GetModuleFileName(NULL,szAppPath,MAX_PATH);
	char * pStr=strrchr(szAppPath,'\\');
	if(pStr!=NULL) 
		*pStr=0x00;

	char szFile[MAX_PATH];
	_snprintf(szFile,MAX_PATH,"%s\\%s",szAppPath,BDHAUDIT_CLIENT);

	if(CUtils::FileExist(szFile))
	{
		//AfxMessageBox(szFile);

		if(m_nWinType>=5)
		{
			//if(!IsUacEnabled())
			{
				bRet=CreateDestopProcess(szFile);
				CUtils::DebugShow("CBDHIDSService::StartClient  CreateDestopProcess:%d",bRet);
			}
		}
		else
		{
			bRet=ExecCommand(szFile,FALSE);
		}
	}

	return bRet;
}

BOOL CBDHIDSService::StartRemoteMng(void)
{
	char szRemoteMng[MAX_PATH];
	char szAppPath[MAX_PATH];
	::GetModuleFileName(NULL,szAppPath,MAX_PATH);
	char * pStr=strrchr(szAppPath,'\\');
	if(pStr!=NULL) 
		*pStr=0x00;

	_snprintf(szRemoteMng,MAX_PATH,"%s\\BDWinVNC.exe",szAppPath);

	BOOL bRet=ExecCommand(szRemoteMng,SW_SHOW,FALSE);
	return bRet;
}

BOOL CBDHIDSService::StartVncSvc(void)
{
  //bdvncsvr
	BOOL bRet=StartSysService("bdvncsvr",FALSE);
	return bRet;
}

BOOL CBDHIDSService::StopClient(void)
{
	BOOL bRet=FALSE;
	HANDLE hEvent=::OpenEvent(EVENT_ALL_ACCESS,TRUE,BDHIDS_EVENT_NOTIFYAPPEXIT);
	if(hEvent==NULL)
	{
		CUtils::DebugShow("StopClient failure because OpenEvent is null");
		return TRUE;
	}
	::SetEvent(hEvent);
	::Sleep(1000);
	::CloseHandle(hEvent);
	CUtils::DebugShow("StopClient ok");
	return bRet;
}




DWORD WINAPI CBDHIDSService::HandlerEx(
									   DWORD dwControl,
									   DWORD dwEventType,
									   LPVOID lpEventData,
									   LPVOID lpContext
									   )
{
	// Get a pointer to the object
	CBDHIDSService* pService = m_pThis;

	OutputDebugString("Come in HandlerEx\n");
	CUtils::DebugMsg("CBDHIDSService::Handler(%lx)", dwControl);

	switch (dwControl) {
	case SERVICE_CONTROL_STOP: // 1
		pService->OnStop();
		break;

	case SERVICE_CONTROL_PAUSE: // 2
		pService->OnPause();
		break;

	case SERVICE_CONTROL_CONTINUE: // 3
		pService->OnContinue();
		break;

	case SERVICE_CONTROL_INTERROGATE: // 4
		pService->OnInterrogate();
		break;

	case SERVICE_CONTROL_SHUTDOWN: // 5
		//		if (!gUpdate)
		//			return;
		{
			FILE * fp=fopen("c:\\WinlogonShutdownEvent.txt","wb+");
			if(fp!=NULL)
			{
				fprintf(fp,"I am WinlogonShutdownEvent\r\n");
				fclose(fp);
			}
		}
		CUtils::DebugShow("SERVICE_CONTROL_SHUTDOWN event\n");
		pService->OnShutdown();
		break;

	case SERVICE_CONTROL_DEVICEEVENT:
		/***********************
		OutputDebugString("SERVICE_CONTROL_DEVICEEVENT\n");

		if(dwEventType == DBT_DEVICEARRIVAL)
		{
		PDEV_BROADCAST_HDR			pdev;
		pdev = (PDEV_BROADCAST_HDR)lpEventData;

		//if(m_hDevChangedEvent!=NULL) ::SetEvent(m_hDevChangedEvent);

		char szMsg[MAX_PATH];
		_snprintf(szMsg,MAX_PATH, "bdhidspred DBT_DEVICEARRIVAL dbch_devicetype[%lx]\n",pdev->dbch_devicetype);
		OutputDebugString(szMsg);

		if(pdev->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
		PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpEventData;

		OutputDebugString(" DBT_DEVTYP_VOLUME\n");
		//if(m_hDiskChangedEvent!=NULL) ::SetEvent(m_hDiskChangedEvent);
		}
		}
		else if(dwEventType == DBT_DEVICEREMOVECOMPLETE)
		{
		PDEV_BROADCAST_HDR			pdev;
		pdev = (PDEV_BROADCAST_HDR)lpEventData;

		OutputDebugString("bdhidspred DBT_DEVICEREMOVECOMPLETE\n");
		if(pdev->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
		OutputDebugString("bdhidspred DISK DBT_DEVICEREMOVECOMPLETE\n");
		}
		}
		********************************/
		break;
	case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
		break;

	case SERVICE_CONTROL_SESSIONCHANGE:
		switch(dwEventType)
		{
		case WTS_SESSION_LOGON:
			// LogOn
			/*
			{
			FILE * fp=fopen("c:\\WinlogonLogonEvent.txt","wb+");
			if(fp!=NULL)
			{
			fprintf(fp,"I am WinlogonLogonEvent\r\n");
			fclose(fp);
			}
			}
			*/

			CUtils::DebugShow("WTS_SESSION_LOGON event");
			/*
			if(!pService->FindMainApp())
			{
			pService->m_bHasStartClient=FALSE;
			}
			*/
			if(pService->m_bHasLogout)
			{
				pService->m_bHasStartClient=FALSE;
				pService->m_bHasLogout=FALSE;
			}
			break;
		case WTS_SESSION_LOGOFF:
			/*
			{
			FILE * fp=fopen("c:\\WinlogonLogoffEvent.txt","wb+");
			if(fp!=NULL)
			{
			fprintf(fp,"I am WinlogonLogoffEvent\r\n");
			fclose(fp);
			}
			}
			*/
			pService->m_bHasLogout=TRUE;
			//MessageBox(NULL,"Logoff","bdhidspred",MB_OK);
			break;
		default:
			break;

		}

		break;
	default:

		if (dwControl >= SERVICE_CONTROL_USER) {
			if (!pService->OnUserControl(dwControl)) {
				//            pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);//原代码直接送事件日志，现改为统一送控制中心。
			}
		} else {
			//         pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);//原代码直接送事件日志，现改为统一送控制中心。
		}
		break;
	}

	// Report current status
	CUtils::DebugMsg("Updating status (%lu, %lu)",
		pService->m_hServiceStatus,
		pService->m_Status.dwCurrentState);
	::SetServiceStatus(pService->m_hServiceStatus, &pService->m_Status);

	return NO_ERROR;
}


void CBDHIDSService::Handler(DWORD dwOpcode)
{
	// Get a pointer to the object
	CBDHIDSService* pService = m_pThis;

	CUtils::DebugMsg("CBDHIDSService::Handler(%lu)", dwOpcode);

	switch (dwOpcode) {
	case SERVICE_CONTROL_STOP: // 1
		pService->OnStop();
		break;

	case SERVICE_CONTROL_PAUSE: // 2
		pService->OnPause();
		break;

	case SERVICE_CONTROL_CONTINUE: // 3
		pService->OnContinue();
		break;

	case SERVICE_CONTROL_INTERROGATE: // 4
		pService->OnInterrogate();
		break;

	case SERVICE_CONTROL_SHUTDOWN: // 5
		//		if (!gUpdate)
		//			return;
		//pService->OnShutdown();
		break;

	case SERVICE_CONTROL_DEVICEEVENT:
		OutputDebugString("SERVICE_CONTROL_DEVICEEVENT\n");
		break;
	case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
		OutputDebugString("SERVICE_CONTROL_HARDWAREPROFILECHANGE\n");
		break;


	default:

		if (dwOpcode >= SERVICE_CONTROL_USER) {
			if (!pService->OnUserControl(dwOpcode)) {
				//            pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);//原代码直接送事件日志，现改为统一送控制中心。
			}
		} else {
			//         pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);//原代码直接送事件日志，现改为统一送控制中心。
		}
		break;
	}

	// Report current status
	CUtils::DebugMsg("Updating status (%lu, %lu)",
		pService->m_hServiceStatus,
		pService->m_Status.dwCurrentState);
	::SetServiceStatus(pService->m_hServiceStatus, &pService->m_Status);
}

BOOL CBDHIDSService::Initialize()
{
	CUtils::DebugMsg("Entering CBDHIDSService::Initialize()");

	// Start the initialization
	m_nWinType=BdGetWindowType();
	SetStatus(SERVICE_START_PENDING);

	// Perform the actual initialization
	BOOL bResult = OnInit(); 

	//     Set final state
	m_Status.dwWin32ExitCode = GetLastError();
	m_Status.dwCheckPoint = 0;
	m_Status.dwWaitHint = 0;

	if(m_nWinType>=3)
	{
		m_Status.dwControlsAccepted |= SERVICE_ACCEPT_SESSIONCHANGE; 
	}

	if (!bResult) {
		//LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILEDINIT);//原代码直接送事件日志，现改为统一送控制中心。
		SetStatus(SERVICE_STOPPED);
		return FALSE;    
	}

	//LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STARTED);//原代码直接送事件日志，现改为统一送控制中心。
	SetStatus(SERVICE_RUNNING);

	CUtils::DebugMsg("Leaving CBDHIDSService::Initialize()");

	return TRUE;
}

BOOL CBDHIDSService::Install()
{
	if (IsInstalled())
		return TRUE;
	// Open the Service Control Manager
	SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
		NULL, // ServicesActive database
		SC_MANAGER_ALL_ACCESS); // full access
	if (hSCM == NULL) return FALSE;

	// Get the executable file path
	char szFilePath[_MAX_PATH];
	//::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));
	::GetModuleFileName(NULL, szFilePath, _MAX_PATH);
	// Create the service
	SC_HANDLE hService = ::CreateService(hSCM,
		m_szServiceName,
		m_szServiceName,
		0,
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_AUTO_START, 
		SERVICE_ERROR_NORMAL,
		szFilePath,
		NULL,
		NULL,
		"\0\0",
		NULL,
		NULL);
	if (hService == NULL) {
		::CloseServiceHandle(hSCM);
		return FALSE;
	}


	// tidy up
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	return TRUE;
}

BOOL CBDHIDSService::IsInstalled()
{
	BOOL bResult = FALSE;

	// Open the Service Control Manager
	SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
		NULL, // ServicesActive database
		SC_MANAGER_ALL_ACCESS); // full access
	if (hSCM != NULL) {

		// Try to open the service
		SC_HANDLE hService = ::OpenService(hSCM,
			m_szServiceName,
			SERVICE_QUERY_CONFIG);
		if (hService != NULL) {
			bResult = TRUE;
			::CloseServiceHandle(hService);
		}

		::CloseServiceHandle(hSCM);
	}

	return bResult;
}




void CBDHIDSService::OnContinue()
{
	CUtils::DebugMsg("CBDHIDSService::OnContinue()");
}

BOOL CBDHIDSService::OnInit()
{
	CUtils::DebugMsg("CBDHIDSService::OnInit()");
	return TRUE;
}

void CBDHIDSService::OnInterrogate()
{
	CUtils::DebugMsg("CBDHIDSService::OnInterrogate()");
}

void CBDHIDSService::OnPause()
{
	CUtils::DebugMsg("CBDHIDSService::OnPause()");
}

void CBDHIDSService::OnShutdown()
{
	CUtils::DebugMsg("CNTService::OnShutdown()");
}

void CBDHIDSService::OnStop()
{
	m_bIsRunning = FALSE;

	CUtils::DebugMsg("CBDHIDSService::OnStop()");
}

BOOL CBDHIDSService::OnUserControl(DWORD dwOpcode)
{
	CUtils::DebugMsg("CBDHIDSService::OnUserControl(%8.8lXH)", dwOpcode);
	return FALSE; // say not handled
}

BOOL CBDHIDSService::ParseStandardArgs(int argc, char *argv[])
{
	if (argc <= 1) return FALSE;
	if (_stricmp(argv[1], "-i") == 0) 
	{
		// Request to install.
		if (IsInstalled()) 
		{
			printf("%s is already installed\n", m_szServiceName);
		} 
		else 
		{
			// Try and install the copy that's running
			if (Install()) 
			{
				printf("%s installed\n", m_szServiceName);
			} 
			else 
			{
				printf("%s failed to install. Error %d\n", m_szServiceName, GetLastError());
			}
		}
		return TRUE; // say we processed the argument

	} 
	else if (_stricmp(argv[1], "-u") == 0) 
	{
		// Request to uninstall.
		if (!IsInstalled()) 
		{
			printf("%s is not installed\n", m_szServiceName);
		} 
		else 
		{
			// Try and remove the copy that's installed
			if (Uninstall()) 
			{
				// Get the executable file path
				char szFilePath[_MAX_PATH];
				::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));
				printf("%s removed.\n",
					m_szServiceName);
			} 
			else 
			{
				printf("Could not remove %s. Error %d\n", m_szServiceName, GetLastError());
			}
		}
		return TRUE; // say we processed the argument
	}


	// Don't recognise the args
	return FALSE;
}

void CBDHIDSService::Run()
{

#ifdef _DEBUGVIEW   
	OutputDebugString("Cwh Enter CBDHIDSService::Run()\n");
#endif
	while (m_bIsRunning) 
	{
		OutputDebugString("CBDHIDSService::Run() Sleeping...\n");
		//Sleep(5000);
		SleeperFunc(5000);
	}

	// nothing more to do
	CUtils::DebugMsg("Leaving CBDHIDSService::Run()");
}

#include <windows.h>
#include <dbt.h>

BOOL CBDHIDSService::DoRegisterDeviceInterface(HANDLE hRecipient, 
											   HDEVNOTIFY *hDevNotify 
											   )
{
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

	ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
	NotificationFilter.dbcc_size = 
		sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	*hDevNotify = RegisterDeviceNotification(hRecipient, 
		&NotificationFilter,
		DEVICE_NOTIFY_SERVICE_HANDLE | 0x00000004);

	if(!(*hDevNotify)) 
	{
		return FALSE;
	}

	return TRUE;
}

void WINAPI CBDHIDSService::ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{

#ifdef _DEBUGVIEW   
	OutputDebugString("Cwh Enter CBDHIDSService::ServiceMain()\n");
#endif


	// Get a pointer to the C++ object
	CBDHIDSService* pService = m_pThis;

	CUtils::DebugMsg("Entering CBDHIDSService::ServiceMain()");
	// Register the control request handler
	pService->m_Status.dwCurrentState = SERVICE_START_PENDING;
	pService->m_hServiceStatus = RegisterServiceCtrlHandlerEx(pService->m_szServiceName,HandlerEx,NULL);

	if(pService->m_hServiceStatus == NULL) 
	{
		//pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_CTRLHANDLERNOTINSTALLED);//原代码直接写事件日志，应改为统一送控制中心。
		OutputDebugString("RegisterServiceCtrlHandlerEx failed!\n");
		return;
	}
	else
	{
		OutputDebugString("RegisterServiceCtrlHandlerEx success!\n");
	}

	/***************
	HDEVNOTIFY hDevNotify;
	BOOL bRet=pService->DoRegisterDeviceInterface(pService->m_hServiceStatus,&hDevNotify); 
	if(!bRet)
	{
	OutputDebugString("RegisterDeviceInterface failed!\n");

	}
	else
	{
	OutputDebugString("RegisterDeviceInterface ok!\n");
	}
	**************/




	// Start the initialisation
	if (pService->Initialize()) 
	{

		// Do the real work. 
		// When the Run function returns, the service has stopped.
		pService->m_bIsRunning = TRUE;
		pService->m_Status.dwWin32ExitCode = 0;
		pService->m_Status.dwCheckPoint = 0;
		pService->m_Status.dwWaitHint = 0;
#ifdef _DEBUGVIEW   
		OutputDebugString("Cwh Enter CBDHIDSService::Run()\n");
#endif
		pService->Run();
	}

	// Tell the service manager we are stopped
	pService->SetStatus(SERVICE_STOPPED);

	CUtils::DebugMsg("Leaving CBDHIDSService::ServiceMain()");

}

void CBDHIDSService::SetStatus(DWORD dwState)
{
	CUtils::DebugMsg("CBDHIDSService::SetStatus(%lu, %lu)", m_hServiceStatus, dwState);
	m_Status.dwCurrentState = dwState;
	::SetServiceStatus(m_hServiceStatus, &m_Status);
}

BOOL CBDHIDSService::SendStatusToSCM(DWORD dwCurrentState, 
									 DWORD dwWin32ExitCode, 
									 DWORD dwServiceSpecificExitCode,
									 DWORD dwCheckPoint, 
									 DWORD dwWaitHint)
{
	BOOL success;

	DWORD dwNum = 0;

	// Fill in all of the SERVICE_STATUS fields
	m_Status.dwServiceType = SERVICE_WIN32;
	m_Status.dwCurrentState = dwCurrentState;

	// Set the control codes the service can receive from SCM.
	// Make sure that the code contains SERVICE_ACCEPT_SESSIONCHANGE.
	// So service will can accept winlogon message.
	/*if (dwCurrentState == SERVICE_START_PENDING)
	MyServiceStatus.dwControlsAccepted = 0;
	else
	MyServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN|
	SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SESSIONCHANGE; */

	// if a specific exit code is defined, set up
	// the win32 exit code properly

	m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SESSIONCHANGE; 
	if (dwServiceSpecificExitCode == 0)
		m_Status.dwWin32ExitCode = dwWin32ExitCode;
	else
		m_Status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;

	m_Status.dwServiceSpecificExitCode = dwServiceSpecificExitCode;    
	m_Status.dwCheckPoint = dwCheckPoint;
	m_Status.dwWaitHint = dwWaitHint;

	// Pass the status record to the SCM
	success = SetServiceStatus (m_hServiceStatus, &m_Status);
	return success;
}

BOOL CBDHIDSService::StartService()
{
	SERVICE_TABLE_ENTRY st[] = 
	{
		{m_szServiceName, ServiceMain},
		{NULL, NULL}
	};
	CUtils::DebugMsg("Calling StartServiceCtrlDispatcher()");

	BOOL b = ::StartServiceCtrlDispatcher(st);

	CString strMsg;
	strMsg.Format("Returned from StartServiceCtrlDispatcher:return %d",b);
	CUtils::DebugMsg(strMsg);
	//	InfoReport.StartHook(TRUE);
	//	InfoReport.SetStartServerFlag(TRUE);
	return b;
}

BOOL CBDHIDSService::Uninstall()
{
	// Open the Service Control Manager
	SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
		NULL, // ServicesActive database
		SC_MANAGER_ALL_ACCESS); // full access
	if (!hSCM) return FALSE;

	BOOL bResult = FALSE;
	SC_HANDLE hService = ::OpenService(hSCM,
		m_szServiceName,
		DELETE);
	if (hService) 
	{
		if (::DeleteService(hService)) 
		{
			//LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_REMOVED, m_szServiceName);//原代码直接送事件日志，现改为统一送控制中心。
			bResult = TRUE;
		} 
		else 
		{
			//LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_NOTREMOVED, m_szServiceName);//原代码直接送事件日志，现改为统一送控制中心。
		}
		::CloseServiceHandle(hService);
	}

	::CloseServiceHandle(hSCM);
	return bResult;
}

void CBDHIDSService::LogEvent(WORD wType, DWORD dwID,
							  const char *pszS1, 
							  const char *pszS2,
							  const char *pszS3)
{

}
