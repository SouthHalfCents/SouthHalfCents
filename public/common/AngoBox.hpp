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
	a)���ɰ���ȫ�ֶ����ȫ�ֺ���
	b)��֮�䲻��ѭ������
	c)����ʹ�þ�̬��Ա
		һ�����н���һ����̬��Աʱ���ҽ���һ��������ʱ������ͨ������̬����ģ��
		����.�����ж��������Ҫ���þ�̬��Ա�����ҿ��ܴ��ڶ����̬��Աʱ�����Խ�ÿ����̬��Ա��װһ��ģ�ⷽ�����������������á�
		�����ڶ��ַ������ڴ󲿷������ͨ�õģ����ǵ�����ľ�̬��Ա����ʱ����д��װ�����Ĺ��������ǳ��޴��ڴ�������£�
			����ʹ��Singletonģʽ�����������ඨ�����ͨ�࣬Ȼ��ʹ��Singleton�����Ϊȫ��Ψһ�Ķ�����е��á�
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
			strMsg.Format(_T("����AngoMsgBox.dllʧ�ܣ�%s\n"),lpMsgBuf);
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

