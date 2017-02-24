#include "stdafx.h"
#include "TrayIcon.h"


CTrayIcon::CTrayIcon()
{
	m_bMin		=	FALSE;
	m_bEnable	=	FALSE;
	m_bTwinkl	=	FALSE;
	m_strTips	=	"tips";
	m_dwIconId	=	0;
	m_pCwnd		=	NULL;

}


CTrayIcon::~CTrayIcon()
{
	//删除该通知状态图标
	::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);
}

BOOL CTrayIcon::InitTrayIcon()
{
	if ( m_bEnable )
		return FALSE;
	if ( !m_pCwnd )
		return FALSE;

	if (m_strTips.GetLength() >= 128)
	{
		m_strTips = m_strTips.Left(126);
	}

	m_ntIcon.cbSize	= sizeof(NOTIFYICONDATA);							//该结构体变量的大小
	m_ntIcon.hIcon	= AfxGetApp()->LoadIcon(m_dwIconId);				//图标，通过资源ID得到
	m_ntIcon.uID	= m_dwIconId;
	m_ntIcon.hWnd	= m_pCwnd->m_hWnd;									//接收托盘图标通知消息的窗口句柄
	wcscpy_s(m_ntIcon.szTip, 128, m_strTips);
	m_ntIcon.uCallbackMessage = MAIN_WM_NOTIFYICON;						//应用程序定义的消息ID号
	m_ntIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;					//图标的属性：设置成员uCallbackMessage、hIcon、szTip有效

	::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);								//在系统通知区域增加这个图标
	m_bEnable = TRUE;

	return TRUE;
}

BOOL CTrayIcon::SetToolTips(CString &strTips)
{
	m_strTips = strTips;
	wcscpy_s(m_ntIcon.szTip, 128, m_strTips);

	if ( !m_bEnable )
		return FALSE;
	
	m_ntIcon.uFlags = NIF_TIP;
	return ::Shell_NotifyIcon(NIM_MODIFY, &m_ntIcon) == TRUE;
}

CString CTrayIcon::GetToolTips()
{
	return m_strTips;
}





BOOL CTrayIcon::SetIcon(DWORD dwResId)
{
	m_dwIconId = dwResId;
	m_ntIcon.hIcon	= AfxGetApp()->LoadIcon(m_dwIconId);
	m_ntIcon.uFlags	= NIF_ICON;
	
	if (!m_bEnable) 
		return FALSE;

	return ::Shell_NotifyIcon(NIM_MODIFY, &m_ntIcon) == TRUE;
}

DWORD CTrayIcon::GetIconId()
{
	return m_dwIconId;
}





//BOOL CTrayIcon::StartTwinkling()
//{
//	if (m_bTwinkl || !m_bEnabled || pIDuiTimer)
//		return TRUE;
//
//	pIDuiTimer = MakeDuiTimer(this, &CDuiTrayIcon::OnTimer, 400);
//	pIDuiTimer->SetDuiTimer();
//	m_bTwinkl = TRUE;
//	return TRUE;
//}
//
//BOOL CTrayIcon::StopTwinkling()
//{
//	if (pIDuiTimer){
//		pIDuiTimer->KillDuiTimer();
//		delete pIDuiTimer;
//		pIDuiTimer = NULL;
//	}
//	m_bTwinkl = FALSE;
//	SetShowIcon();
//	return TRUE;
//}