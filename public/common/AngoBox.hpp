#pragma once

#include <afxwin.h>
#include "AngoBoxDefine.h"

#ifndef MODULES_PATH

#if _WIN64
#define  MODULES_PATH _T("Modules")
#else
#define  MODULES_PATH _T("Modules(x86)")
#endif

#endif // !MODULES_PATH

/*
hpp:
	a)不可包含全局对象和全局函数
	b)类之间不可循环调用
	c)不可使用静态成员
		一、类中仅有一个静态成员时，且仅有一个调用者时，可以通过局域静态变量模拟
		二、.类中有多个方法需要调用静态成员，而且可能存在多个静态成员时，可以将每个静态成员封装一个模拟方法，供其他方法调用。
		三、第二种方法对于大部分情况是通用的，但是当所需的静态成员过多时，编写封装方法的工作量将非常巨大，在此种情况下，
			建议使用Singleton模式，将被调用类定义成普通类，然后使用Singleton将其变为全局唯一的对象进行调用。
*/


class CAngoBox
{
private:
	CAngoBox(void)
	{
		m_bInit = FALSE;
		m_pfAngoMessageBox = NULL;
		m_strModulePath = "";
	};
	~CAngoBox(void){};
	CAngoBox(const CAngoBox&);										//Not Implemented
	CAngoBox & operator=(const CAngoBox&);							//Not Implemented


	typedef int( *ANGO_MESSAGE_BOX)(CString strMsg, CString strTitle, UINT nType);

	BOOL					m_bInit;
	ANGO_MESSAGE_BOX		m_pfAngoMessageBox;
	CString					m_strModulePath;

	BOOL Init()
	{
		if (m_bInit)
			return m_bInit;

		TCHAR szLocalPath[MAX_PATH];
		GetModuleFileName(0, szLocalPath, MAX_PATH);

		TCHAR szFullPath[4096] = { 0 };
		GetFullPathName(szLocalPath, 4096, szFullPath, NULL);

		CString strTemp;
		
		if (m_strModulePath.GetLength() == 0)
		{
				strTemp = szFullPath;
			strTemp = strTemp.Left(strTemp.ReverseFind('\\'));
			strTemp += _T("\\");
			strTemp += MODULES_PATH;
			strTemp += _T("\\AngoMsgBox.dll");
		}
		else
		{
			strTemp += m_strModulePath;
			strTemp += _T("\\AngoMsgBox.dll");
		}

		HMODULE hDll = LoadLibrary(strTemp);
		if (!hDll)
		{
			LPVOID lpMsgBuf;
			TCHAR szError[1024] = {0};
			DWORD dwError = GetLastError();
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL
				);
			CString strMsg;
			strMsg.Format(_T("加载AngoMsgBox.dll失败，%s\n"),lpMsgBuf);
			OutputDebugString(strMsg);
			return false;
		}
			

		m_pfAngoMessageBox = (ANGO_MESSAGE_BOX)GetProcAddress(hDll, "AngoMessageBox");
		if (m_pfAngoMessageBox)
		{
			m_bInit = TRUE;
		}
		return m_bInit;
	}

public:
	int AngoMessageBox(CString strMsg, CString strTitle = _T(""), UINT nType = MB_POST_BESIDE | MB_OK )
	{
		if (!m_bInit)
		{
			Init();
		}

		if (m_pfAngoMessageBox)
		{
			return m_pfAngoMessageBox(strMsg, strTitle, nType);
		}
		else
		{
			return -1;
		}
	}


	
public:
	static CAngoBox& CAngoBox::GetInstance(CString strTemp = _T(""))
	{
		static CAngoBox angoBox;
		if (strTemp.GetLength() != 0)
		{
			angoBox.m_strModulePath = strTemp;
		}
		
		return angoBox;
	}
};

