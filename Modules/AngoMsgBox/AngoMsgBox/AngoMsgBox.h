// AngoMsgBox.h : AngoMsgBox DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAngoMsgBoxApp
// �йش���ʵ�ֵ���Ϣ������� AngoMsgBox.cpp
//

class CAngoMsgBoxApp : public CWinApp
{
public:
	CAngoMsgBoxApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
