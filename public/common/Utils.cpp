
#include "Utils.h"


CUtils::CUtils()
{
}


CUtils::~CUtils()
{
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
	if (GetFileAttributes(pszFileName) == -1)
	{
		return FALSE;
	}
	return TRUE;
}


CString CUtils::GetAppPath()
{
	TCHAR szLocalPath[MAX_PATH];
	GetModuleFileName(0, szLocalPath, MAX_PATH);

	TCHAR szFullPath[4096] = { 0 };
	GetFullPathName(szLocalPath, 4096, szFullPath, NULL);

	CString strTemp = szFullPath;
	return strTemp;
}

CString CUtils::GetAppDir()
{
	TCHAR szLocalPath[MAX_PATH];
	GetModuleFileName(0, szLocalPath, MAX_PATH);

	TCHAR szFullPath[4096] = { 0 };
	GetFullPathName(szLocalPath, 4096, szFullPath, NULL);

	CString strTemp = szFullPath;
	strTemp = strTemp.Left(strTemp.ReverseFind('\\'));
	return strTemp;
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

///< 将GBK字符串转化为UTF8编码
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
///< 将UTF8编码转化为GBK编码
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

/*
BOOL bEnable: TRUE 增加自启动  FALSE 删除自启动 
*/
BOOL CUtils::SetRegAutoStart(BOOL bEnable, CString strKeyName, CString strKeyValue)
{
	if (strKeyName.GetLength() == 0 || strKeyValue.GetLength() == 0)
		return FALSE;

	BOOL bResult = TRUE;
	BOOL bIsWow64 = IsWow64();
	HKEY hRoot = HKEY_CURRENT_USER;			// HKEY_LOCAL_MACHINE 也可以
	HKEY hKey = 0;
	DWORD dwRetValue = 0;
	DWORD dwRetParam = 0;

	TCHAR * pKeyName  = strKeyName.GetBuffer();
	TCHAR * pKeyValue = strKeyValue.GetBuffer();
	DWORD dwKeyValueLen = (DWORD)wcslen(pKeyValue) * sizeof(TCHAR);
	
	//HKEY_CURRENT_USER "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#if _WIN64
	const TCHAR *subkey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");	//RunOnce
#else
	//64位系统下而32位注册表项被重定位到：   HKEY_LOCAL_MACHINE\Software\WOW6432Node
	const TCHAR *subkey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");	//RunOnce
#endif


	dwRetValue = RegCreateKeyEx(hRoot,
		subkey,
		0,   //保留参数，必须为0
		NULL,//键的种类，同通常为NULL
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwRetParam  //返回参数
		);


	//自启动？删除自启动
	if (bEnable)
	{
		dwRetValue = ::RegSetValueEx(
			hKey,
			pKeyName,
			0,							//强制保留
			REG_SZ,						//宽字符串
			(CONST BYTE *)pKeyValue,	//具体内容
			dwKeyValueLen
			);
	}
	else
	{
		dwRetValue = ::RegDeleteValue(hKey, pKeyName);
	}

	if (dwRetValue != ERROR_SUCCESS)
	{
		OutputDebugString(_T("打开注册表失败\n"));
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

//如果bCheckAdminMode是TRUE， 则除了检测Admin账号外，还检测是真的运行在Admin环境， 否则只是检测Admin账号。
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
		OutputDebugString(_T("获取令牌句柄失败!")); 
		return FALSE;
	}

	TOKEN_PRIVILEGES tp; //新特权结构体    
	LUID Luid;
	bReturn = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid);

	if (!bReturn)
	{
		OutputDebugString(_T("获取Luid失败"));
		CloseHandle(hToken);
		return FALSE;
	}

	//给TP和TP里的LUID结构体赋值    
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = Luid;

	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	if (GetLastError() != ERROR_SUCCESS)
	{
		OutputDebugString(_T("修改特权不完全或失败!"));
		bReturn = FALSE;
	}
	else
	{
		OutputDebugString(_T("修改成功!"));
		bReturn = TRUE;
	}

	CloseHandle(hToken);
	return bReturn;
}