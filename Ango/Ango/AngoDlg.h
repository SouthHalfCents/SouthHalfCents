
// AngoDlg.h : ͷ�ļ�
//

#pragma once
#include "CustomView.h"
#include "./common/log.h"
#include "./common/TrayIcon.h"
#include "ConfigMng.h"

// CAngoDlg �Ի���
class CAngoDlg : public CDialogEx
{
// ����
public:
	CAngoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ANGO_MAIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON						m_hIcon;

	//// 
	virtual BOOL				OnInitDialog();
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void				OnClose();
	afx_msg void				OnPaint();
	afx_msg HCURSOR				OnQueryDragIcon();

	///// ��Ӧ�˵�
	afx_msg void				OnToolCustom();
	afx_msg void				OnConfigCustom();
	afx_msg void				OnMenuAbout();
	afx_msg void				OnMenuExit();

	afx_msg void				OnMenuUp();
	afx_msg void				OnMenuDown();
	afx_msg void				OnMenuShow();
	afx_msg void				OnMenuHide();
	
	
	
	//// ��Ӧ����
	virtual void				OnOK();							//����
	virtual void				OnCancel();						//����
	afx_msg void				OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	

	//// �Զ�����Ϣ
	afx_msg LRESULT				OnNotifyIcon(WPARAM, LPARAM);
	virtual void				WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	DECLARE_MESSAGE_MAP()



public:
	CCustomView					m_cusView;				//���Ի�����
	CTrayIcon					m_trayIcon;				//֪ͨ����ͼ��
	CConfigMng*					m_pCfgMng;				//���ù���
	
	BOOL						RegAutoStart();
	CMenu						m_popMenu;

	HANDLE						m_hOnlyMutex;

	afx_msg void				OnAngoTime();			//����AngoTime
	
};
