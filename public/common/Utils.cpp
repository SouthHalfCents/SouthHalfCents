
#include "Utils.h"

#include <tlhelp32.h>
#pragma warning(disable:4996)

CUtils::CUtils()
{
}


CUtils::~CUtils()
{
}

void CUtils::DebugMsg(const char* pszFormat, ...)
{
#ifdef _DEBUG
	char buf[8192] = { 0 };
	char date[MAX_PATH] = { 0 };
	char time[MAX_PATH] = { 0 };
	char name[MAX_PATH] = { 0 };

	CStringA strName = GetAppName();
	strncpy(name, strName, MAX_PATH);

	SYSTEMTIME st;
	GetLocalTime(&st);
	GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "yyyy'-'MM'-'dd", date, sizeof(date));
	GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "HH':'mm':'ss", time, sizeof(time));

	_snprintf(buf, 8192, "[%s](%s %s):", name, date, time);
	//_snprintf(buf, 8192, "[BDAServer](%lu - %s %s): ", GetCurrentThreadId(),date,time);
	va_list arglist;
	va_start(arglist, pszFormat);
	_vsnprintf(&buf[strlen(buf)], 8192 - strlen(buf) - 1, pszFormat, arglist);
	va_end(arglist);
	_snprintf(buf, 8192, "%s\n", buf);
	OutputDebugStringA(buf);
#endif

}

void CUtils::DebugShow(const char* pszFormat, ...)
{

#ifdef _DEBUG
	char buf[8192] = {0};
	char date[MAX_PATH]  = {0};
	char time[MAX_PATH]  = {0};
	char name[MAX_PATH]  = {0};
	CStringA strPath = GetAppDir();
	CStringA strName = GetAppName();
	strncpy(name,strName,MAX_PATH);

	SYSTEMTIME st;
	GetLocalTime(&st);
	GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "yyyy'-'MM'-'dd", date, sizeof(date));
	GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "HH':'mm':'ss", time, sizeof(time));

	_snprintf(buf, 8192, "[%s](%s %s):", name, date, time);
	//_snprintf(buf, 8192, "[BDAServer](%lu - %s %s): ", GetCurrentThreadId(),date,time);
	va_list arglist;
	va_start(arglist, pszFormat);
	_vsnprintf(&buf[strlen(buf)], 8192 - strlen(buf) - 1, pszFormat, arglist);
	va_end(arglist);
	_snprintf(buf, 8192, "%s\n", buf);
	OutputDebugStringA(buf);

	strPath += "\\" + strName;
	strPath = strPath.Left(strPath.ReverseFind('.'));
	strPath += ".txt";
	FILE *fp = fopen(strPath, "a+");
	if (fp == NULL)
	{
		DebugMsg(CStringA(GetLastErrorStr()));
		return;
	}

	fprintf(fp, buf);
	fclose(fp);
#endif
	
}

int CUtils::TranDayweekToInt(int nDayofWeek, int & nDayValue)
{
	if (nDayofWeek <= 0 || nDayofWeek > 7)
	{
		nDayValue = 0;
		return nDayValue;
	}

	switch (nDayofWeek)
	{
	case 1:
		nDayValue = DAY_SUNDAY;
		break;
	case 2:
		nDayValue = DAY_MONDAY;
		break;
	case 3:
		nDayValue = DAY_TUESDAY;
		break;
	case 4:
		nDayValue = DAY_WEDNESDAY;
		break;
	case  5:
		nDayValue = DAY_THURSDAY;
		break;
	case  6:
		nDayValue = DAY_FRIDAY;
		break;
	case 7:
		nDayValue = DAY_SATURDAY;
		break;

	default:
		nDayValue = DAY_NONE;
		break;
	}

	return nDayValue;
}

CString CUtils::FileNameFromPath(CString strPath)
{
	CString filename;

	int pos = strPath.ReverseFind('\\');
	if (pos == -1)
		return strPath;

	filename = strPath.Mid(pos + 1);
	return filename;
}

BOOL CUtils::FileExist(LPCTSTR   pszFileName)
{
	//BOOL bRet=::PathFileExists(pszFileName);
	if (GetFileAttributes(pszFileName) == -1)
	{
		return FALSE;
	}
	return TRUE;
}


CString CUtils::GetAppPath()
{
	TCHAR szLocalPath[MAX_PATH] = {0};
	GetModuleFileName(0, szLocalPath, MAX_PATH);

	TCHAR szFullPath[4096] = { 0 };
	GetFullPathName(szLocalPath, 4096, szFullPath, NULL);

	CString strTemp = szFullPath;
	return strTemp;
}

CString CUtils::GetAppDir()
{
	TCHAR szLocalPath[MAX_PATH] = {0};
	GetModuleFileName(0, szLocalPath, MAX_PATH);

	TCHAR szFullPath[4096] = { 0 };
	GetFullPathName(szLocalPath, 4096, szFullPath, NULL);

	CString strTemp = szFullPath;
	strTemp = strTemp.Left(strTemp.ReverseFind('\\'));
	return strTemp;
}

CString CUtils::GetAppName()
{
	TCHAR szLocalPath[MAX_PATH] = { 0 };
	GetModuleFileName(0, szLocalPath, MAX_PATH);

	CString strTemp = szLocalPath;
	int nPos = strTemp.ReverseFind('\\') + 1;
	int nLen = strTemp.GetLength();
	strTemp = strTemp.Right(nLen - nPos);
	return strTemp;

// 	TCHAR szLocalPath[MAX_PATH] = { 0 };
// 	GetModuleFileName(0, szLocalPath, MAX_PATH);
// 
// 	CString strTemp = szLocalPath;
// 	int nPos = strTemp.ReverseFind('\\') + 1;
// 	int nLen = strTemp.GetLength();
// 	strTemp = strTemp.Right(nLen - nPos);
// 	return strTemp;
}

BOOL CUtils::IsFileType(LPCTSTR lpsz, LPCTSTR lpszType)
{
	CString strType = lpszType;
	CString str = lpsz;
	if (str.GetLength() >= 5){
		int nFind = str.ReverseFind('.');
		if (nFind != -1) {

			str = str.Right(str.GetLength() - nFind);
			str.MakeLower();

			if (str == strType){
				return TRUE;
			}

		}
	}
	return FALSE;
}

///< ��GBK�ַ���ת��ΪUTF8����
std::string	CUtils::GBK_UTF8(std::string& strMsg)
{
	return GBK_UTF8((char*)strMsg.c_str());
}

std::string	CUtils::GBK_UTF8(char* strMsg)
{
	if (strMsg == NULL)
	{
		return "";
	}

	std::string strOutUTF8 = "";

	int nWsize = MultiByteToWideChar(CP_ACP, 0, strMsg, -1, NULL, 0);
	WCHAR * strW = new WCHAR[nWsize + 1];
	strW[nWsize] = 0;
	MultiByteToWideChar(CP_ACP, 0, strMsg, -1, strW, nWsize);
	int nMSize = WideCharToMultiByte(CP_UTF8, 0, strW, -1, NULL, 0, NULL, NULL);
	char * strM = new char[nMSize + 1];
	strM[nMSize] = 0;
	WideCharToMultiByte(CP_UTF8, 0, strW, -1, strM, nMSize, NULL, NULL);
	strOutUTF8 = strM;
	delete[]strW;
	delete[]strM;
	return strOutUTF8;
}
///< ��UTF8����ת��ΪGBK����
std::string CUtils::UTF8_GBK(std::string& strMsg)
{
	return UTF8_GBK((char*)strMsg.c_str());
}

std::string	CUtils::UTF8_GBK(char* strMsg)
{
	if (strMsg == NULL)
	{
		return "";
	}

	std::string strOutGBK = "";

	int nWsize = MultiByteToWideChar(CP_UTF8, 0, strMsg, -1, NULL, 0);
	WCHAR * strW = new WCHAR[nWsize + 1];
	strW[nWsize] = 0;
	MultiByteToWideChar(CP_UTF8, 0, strMsg, -1, strW, nWsize);
	int nMSize = WideCharToMultiByte(CP_ACP, 0, strW, -1, NULL, 0, NULL, NULL);
	char * strM = new char[nMSize + 1];
	strM[nMSize] = 0;
	WideCharToMultiByte(CP_ACP, 0, strW, -1, strM, nMSize, NULL, NULL);
	strOutGBK = strM;
	delete[]strW;
	delete[]strM;
	return strOutGBK;
}


CString CUtils::GetLastErrorStr()
{
	DWORD dwError = GetLastError();
	CString strMsg;
	strMsg.Format(_T("%d:"), dwError);
	strMsg += GetStrFromError(dwError);
	return strMsg;
}

CString CUtils::GetStrFromError(DWORD dwError)
{
	CString strError;
	LPTSTR lpMsgBuf = NULL;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
		);

	if (lpMsgBuf == NULL)
	{
		return strError = "��������ʧ�ܣ������ڴ治�ɹ�\r\n";
	}

	strError = lpMsgBuf;
	LocalFree(lpMsgBuf);
	lpMsgBuf = NULL;
	return strError;
}

/*
BOOL bEnable: TRUE ����������  FALSE ɾ�������� 
*/
BOOL CUtils::SetRegAutoStart(BOOL bEnable, CString strKeyName, CString strKeyValue)
{
	if (strKeyName.GetLength() == 0 || strKeyValue.GetLength() == 0)
		return FALSE;

	BOOL bResult = TRUE;
	BOOL bIsWow64 = IsWow64();
	HKEY hRoot = HKEY_CURRENT_USER;			// HKEY_LOCAL_MACHINE Ҳ����
	HKEY hKey = 0;
	DWORD dwRetValue = 0;
	DWORD dwRetParam = 0;

	TCHAR * pKeyName  = strKeyName.GetBuffer();
	TCHAR * pKeyValue = strKeyValue.GetBuffer();
	DWORD dwKeyValueLen = (DWORD)lstrlen(pKeyValue) * sizeof(TCHAR);


	
	
	//HKEY_CURRENT_USER "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#if _WIN64
	const TCHAR *subkey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");	//RunOnce
#else
	//64λϵͳ�¶�32λע�����ض�λ����   HKEY_LOCAL_MACHINE\Software\WOW6432Node
	const TCHAR *subkey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");	//RunOnce
#endif


	dwRetValue = RegCreateKeyEx(hRoot,
		subkey,
		0,   //��������������Ϊ0
		NULL,//�������࣬ͬͨ��ΪNULL
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwRetParam  //���ز���
		);


	//��������ɾ��������
	if (bEnable)
	{
		dwRetValue = ::RegSetValueEx(
			hKey,
			pKeyName,
			0,							//ǿ�Ʊ���
			REG_SZ,						//���ַ���
			(CONST BYTE *)pKeyValue,	//��������
			dwKeyValueLen
			);
	}
	else
	{
		dwRetValue = ::RegDeleteValue(hKey, pKeyName);
	}

	if (dwRetValue != ERROR_SUCCESS)
	{
		OutputDebugString(_T("��ע���ʧ��\n"));
		bResult = FALSE;
	}

	strKeyName.ReleaseBuffer();
	strKeyValue.ReleaseBuffer();

	RegCloseKey(hKey);
	return bResult;
}

BOOL CUtils::IsWow64()
{
	BOOL bRet = FALSE;

	SYSTEM_INFO si;

	typedef VOID(__stdcall*GETNATIVESYSTEMINFO)(LPSYSTEM_INFO lpSystemInfo);

	GETNATIVESYSTEMINFO fnGetNativeSystemInfo;
	fnGetNativeSystemInfo = (GETNATIVESYSTEMINFO)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if (fnGetNativeSystemInfo != NULL)
	{
		fnGetNativeSystemInfo(&si);

		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||

			si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)

		{
			bRet = TRUE;
		}
	}

	return bRet;
}

//BOOL bOnly  �Ƿ�ֻҪ��������
BOOL CUtils::ShellExecute(HWND hwnd, LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd, BOOL bOnly/*=FALSE*/)
{
	BOOL bRet = FALSE;
	CString strMsg;
	CString strOpration = lpOperation;
	do 
	{
		if ( bOnly  && strOpration=="open" && lpFile!=NULL )
		{
			CString strTarget = FileNameFromPath(lpFile);
			HANDLE hToken = NULL;
			DWORD dwSessionId = 0;
			if (FindProcess(hToken, dwSessionId, strTarget))
			{
				bRet = TRUE;
				strMsg = "�ⲿ����" + strTarget + "�Ѿ���������";
				DebugShow((CStringA)strMsg);
				break;
			}
		}

		HINSTANCE hInstance = ::ShellExecute(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
		if ((DWORD)hInstance <= 32)
		{
			strMsg = "ִ��ShellExecuteʧ�ܣ�"+GetShellExuecuteMsg((DWORD)hInstance);
			DebugShow((CStringA)strMsg);
			break;
		}


		bRet = TRUE;
	} while (FALSE);


	return bRet;
}

BOOL CUtils::FindProcess(HANDLE& hToken, DWORD& dwSessionId, CString strName)
{

	BOOL bIsFind = FALSE;
	CString strExeFile;
	//notice:only use in win2000/winxp/win2003
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnapshot)
	{
		CUtils::DebugShow("Leave FindProcess::CreateToolhelp32Snapshot failed");
		return FALSE;
	}


	// prepare to enumerate the processes....
	PROCESSENTRY32 pe;

	// remember to init the structure...
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL retval = Process32First(hSnapshot, &pe);
	while (retval)
	{
		strExeFile = pe.szExeFile;
		//if (!_stricmp(pe.szExeFile,/*"winlogon.exe"*/  "explorer.exe"  /*"bdhidspred.exe"*/))
		if (strExeFile == strName)
		{
			CUtils::DebugShow("IN CBDHIDSCoreService::FindDestopProc=> Process id:%ld  name:%s\n", pe.th32ProcessID, pe.szExeFile);
			dwSessionId = 0L;

			ProcessIdToSessionId(pe.th32ProcessID, &dwSessionId);

			HANDLE hCurToken = NULL;
			LUID DebugNameValue;
			TOKEN_PRIVILEGES Privileges;
			DWORD dwRet;
			OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hCurToken);
			LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &DebugNameValue);
			Privileges.PrivilegeCount = 1;
			Privileges.Privileges[0].Luid = DebugNameValue;
			Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hCurToken, FALSE, &Privileges, sizeof(Privileges), NULL, &dwRet);
			CloseHandle(hCurToken);


			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
				FALSE, pe.th32ProcessID);
			BOOL bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);

			//hToken=GetToken(pe.th32ProcessID);
			bIsFind = TRUE;
			break;
		}
		retval = Process32Next(hSnapshot, &pe);
	}

	// since we are done, remember to close the snapshot handle
	CloseHandle(hSnapshot);
	return bIsFind;
}


CString CUtils::GetShellExuecuteMsg(DWORD dwError)
{
	CString strMsg;
	do 
	{
		if (dwError > 32)
		{
			strMsg = "�ɹ�ִ��";
			break;
		}

		switch (dwError)
		{
		case 0:
		{
			strMsg = "{0:�ڴ治��}";
			break;
		}
		case ERROR_FILE_NOT_FOUND:
		{
			strMsg = "{2:�ļ�������}";
			break;
		}
		case ERROR_PATH_NOT_FOUND:
		{
			strMsg = "{3:·��������}";
			break;
		}
		case SE_ERR_ACCESSDENIED:
		{
			strMsg = "{5:Ȩ�޲��㣬�޷������ļ�}";
			break;
		}
		case ERROR_BAD_FORMAT:
		{
			strMsg = "{11:EXE �ļ���Ч}";
			break;
		}
		case SE_ERR_SHARE:
		{
			strMsg = "{26:�����������}";
			break;
		}
		case SE_ERR_ASSOCINCOMPLETE:
		{
			strMsg = "{27:�ļ�������ȫ����Ч}";
			break;
		}
		case SE_ERR_DDETIMEOUT:
		{
			strMsg = "{28:��ʱ}";
			break;
		}
		case SE_ERR_DDEFAIL:
		{
			strMsg = "{29:DDE ����ʧ��}";
			break;
		}
		case SE_ERR_DDEBUSY:
		{
			strMsg = " {30:���ڴ������� DDE �����������ɸ� DDE ����}";
			break;
		}
		case SE_ERR_NOASSOC:
		{
			strMsg = "{31:û���������Ӧ�ó���}";
			break;
		}
		case SE_ERR_DLLNOTFOUND:
		{
			strMsg = "{32:DLL �ļ���Ч}";
			break;
		}
		default:
			strMsg.Format(_T("{%d:��������}"),dwError);
			break;
		}


	} while (FALSE);

	return strMsg;

}


BOOL CUtils::IsRunAsAdmin()
{
	BOOL bElevated = FALSE;
	HANDLE hToken = NULL;

	// Get current process token
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return FALSE;

	TOKEN_ELEVATION tokenEle;
	DWORD dwRetLen = 0;

	// Retrieve token elevation information
	if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen))
	{
		if (dwRetLen == sizeof(tokenEle))
		{
			bElevated = tokenEle.TokenIsElevated;
		}
	}

	CloseHandle(hToken);
	return bElevated;
}

//���bCheckAdminMode��TRUE�� ����˼��Admin�˺��⣬����������������Admin������ ����ֻ�Ǽ��Admin�˺š�
BOOL CUtils::IsInAdminGroup(BOOL bCheckAdminMode /*= FALSE*/)
{
	BOOL   fAdmin;
	HANDLE  hThread;
	TOKEN_GROUPS *ptg = NULL;
	DWORD  cbTokenGroups;
	DWORD  dwGroup;
	PSID   psidAdmin;
	SID_IDENTIFIER_AUTHORITY SystemSidAuthority = SECURITY_NT_AUTHORITY;
	if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, &hThread))
	{
		if (GetLastError() == ERROR_NO_TOKEN)
		{
			if (! OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY,&hThread))
				return (FALSE);
		}
		else
			return (FALSE);
	}
	if (GetTokenInformation(hThread, TokenGroups, NULL, 0, &cbTokenGroups))
		return (FALSE);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		return (FALSE);
	if (! (ptg = (TOKEN_GROUPS*)_alloca(cbTokenGroups)))
		return (FALSE);
	if (!GetTokenInformation(hThread, TokenGroups, ptg, cbTokenGroups,
		&cbTokenGroups))
		return (FALSE);
	if (! AllocateAndInitializeSid(&SystemSidAuthority, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0, &psidAdmin))
		return (FALSE);
	fAdmin = FALSE;
	for (dwGroup = 0; dwGroup < ptg->GroupCount; dwGroup++)
	{
		if (EqualSid(ptg->Groups[dwGroup].Sid, psidAdmin))
		{
			if (bCheckAdminMode)
			{
				if ((ptg->Groups[dwGroup].Attributes) & SE_GROUP_ENABLED)
				{
					fAdmin = TRUE;
				}
			}
			else
			{
				fAdmin = TRUE;
			}
			break;
		}
	}

	CloseHandle(hThread);
	FreeSid(psidAdmin);
	return (fAdmin);
}


//EnablePrivilege(token, SE_SHUTDOWN_NAME);
BOOL CUtils::EnablePrivilege(HANDLE hToken, LPCTSTR lpszPrivilegeName)
{
	TOKEN_PRIVILEGES tkp = { 0 };
	BOOL bRet = LookupPrivilegeValue(NULL, lpszPrivilegeName, &tkp.Privileges[0].Luid);
	if (!bRet) return FALSE;

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	bRet = AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL);

	return bRet;
}

BOOL CUtils::UpPrivilege()
{
	BOOL bReturn = FALSE;
	HANDLE hToken= NULL;
	bReturn = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	if (!bReturn)
	{
		OutputDebugString(_T("��ȡ���ƾ��ʧ��!")); 
		return FALSE;
	}

	TOKEN_PRIVILEGES tp; //����Ȩ�ṹ��    
	LUID Luid;
	bReturn = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid);

	if (!bReturn)
	{
		OutputDebugString(_T("��ȡLuidʧ��"));
		CloseHandle(hToken);
		return FALSE;
	}

	//��TP��TP���LUID�ṹ�帳ֵ    
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = Luid;

	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	if (GetLastError() != ERROR_SUCCESS)
	{
		OutputDebugString(_T("�޸���Ȩ����ȫ��ʧ��!"));
		bReturn = FALSE;
	}
	else
	{
		OutputDebugString(_T("�޸ĳɹ�!"));
		bReturn = TRUE;
	}

	CloseHandle(hToken);
	return bReturn;
}