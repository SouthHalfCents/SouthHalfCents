
// YY520.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CYY520App: 
// �йش����ʵ�֣������ YY520.cpp
//

class CYY520App : public CWinApp
{
public:
	CYY520App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	
};

extern CYY520App theApp;