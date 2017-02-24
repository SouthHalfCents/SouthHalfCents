#pragma once

//// 自定义界面，自定义对话框
//// 创建个性化对话框

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
	void					InitCusView();				//初始化自定义界面
	void					OnPaint();					//OnPaint重绘界面
	void					SetNextView();
	void					SetTargetView(DWORD dwID);

	vector<DWORD>			m_vBitmapID;				
	DWORD					m_dwCurrent;
public:
	void					ReInitCustView();			//运行过程中重新初始化修改界面
	HRGN					BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance=NULL);	//加载BMP到显示区域
};

