
// YY520Time.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CYY520TimeApp: 
// �йش����ʵ�֣������ YY520Time.cpp
//

class CYY520TimeApp : public CWinApp
{
public:
	CYY520TimeApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	
};

extern CYY520TimeApp theApp;