
// AngoDlg.h : ͷ�ļ�
//

#pragma once
#include "CustomView.h"
#include "TrayIcon.h"
#include "ConfigMng.h"
#include "log.h"

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
	afx_msg void				OnPaint();
	afx_msg HCURSOR				OnQueryDragIcon();

	///// ��Ӧ�˵�
	afx_msg void				OnShowDlg();
	afx_msg void				OnAboutDlg();
	afx_msg void				OnExitDlg();
	afx_msg void				OnConfig();
	
	//// ��Ӧ����
	virtual void				OnOK();							//����
	virtual void				OnCancel();						//����
	afx_msg void				OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void				OnUpLayer();

	//// �Զ�����Ϣ
	afx_msg LRESULT				OnNotifyIcon(WPARAM, LPARAM);
	virtual void				WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	DECLARE_MESSAGE_MAP()



public:
	BOOL						m_bUplayer;				//�����ʾ
	CCustomView					m_cusView;				//���Ի�����
	CTrayIcon					m_trayIcon;				//֪ͨ����ͼ��


	CConfigMng*					m_pCfgMng;				//���ù���
	
	BOOL						RegAutoStart();
};
