#pragma once

//// ����ͼ�꣬������ͼ�֪꣬ͨ����ͼ��
#include "Database.h"

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

	BOOL						m_bEnable;				//�Ƿ��Ѿ�����
	BOOL						m_bMin;					//�Ƿ���С��״̬
	BOOL						m_bTwinkl;				//�Ƿ���˸
	DWORD						m_dwIconId;				//ͼ��id
	CWnd*						m_pCwnd;				//��Ӧ������ָ��
	CString						m_strTips;				//������ʾ


protected:
	NOTIFYICONDATA				m_ntIcon;				//���֪ͨ����ͼ��
	
};

