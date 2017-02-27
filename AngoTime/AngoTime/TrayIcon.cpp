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
	//ɾ����֪ͨ״̬ͼ��
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

	m_ntIcon.cbSize	= sizeof(NOTIFYICONDATA);							//�ýṹ������Ĵ�С
	m_ntIcon.hIcon	= AfxGetApp()->LoadIcon(m_dwIconId);				//ͼ�꣬ͨ����ԴID�õ�
	m_ntIcon.uID	= m_dwIconId;
	m_ntIcon.hWnd	= m_pCwnd->m_hWnd;									//��������ͼ��֪ͨ��Ϣ�Ĵ��ھ��
	wcscpy_s(m_ntIcon.szTip, 128, m_strTips);
	m_ntIcon.uCallbackMessage = MAIN_WM_NOTIFYICON;						//Ӧ�ó��������ϢID��
	m_ntIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;					//ͼ������ԣ����ó�ԱuCallbackMessage��hIcon��szTip��Ч

	::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);								//��ϵͳ֪ͨ�����������ͼ��
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