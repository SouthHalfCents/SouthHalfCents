#pragma once

//// �Զ�����棬�Զ���Ի���
//// �������Ի��Ի���

#include "Database.h"
#include "resource.h"
#include <vector>

using std::vector;

class CCustomView
{
public:
	CCustomView();
	~CCustomView();

	CWnd*					m_pCwnd;
	CBitmap					m_Bitmap;
	void					InitBmpVector(vector<DWORD> &vData);
	void					InitCusView();				//��ʼ���Զ������
	void					OnPaint();					//OnPaint�ػ����
	void					SetNextView();
	void					SetTargetView(DWORD dwID);

	vector<DWORD>			m_vBitmapID;				
	DWORD					m_dwCurrent;
public:
	void					ReInitCustView();			//���й��������³�ʼ���޸Ľ���
	HRGN					BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance=NULL);	//����BMP����ʾ����
};

