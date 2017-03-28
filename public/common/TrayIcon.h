#pragma once

//// 托盘图标，任务栏图标，通知区域图标
#include <afxwin.h>

#define MAIN_WM_NOTIFYICON		(WM_USER+100)		//通知区域图标消息

class CTrayIcon
{
public:
	CTrayIcon();
	~CTrayIcon();

	BOOL						InitTrayIcon();

	BOOL						SetToolTips(CString &strTips);
	CString						GetToolTips();
	BOOL						SetIcon(DWORD dwResId);
	DWORD						GetIconId();
// 	BOOL						StartTwinkling();
// 	BOOL						StopTwinkling();

	BOOL						m_bEnable;				//是否已经创建
	BOOL						m_bMin;					//是否最小化状态
	BOOL						m_bTwinkl;				//是否闪烁
	DWORD						m_dwIconId;				//图标id
	CWnd*						m_pCwnd;				//对应主窗口指针
	CString						m_strTips;				//浮动提示


protected:
	NOTIFYICONDATA				m_ntIcon;				//添加通知区域图标
	
};

