
// AngoDlg.h : 头文件
//

#pragma once
#include "CustomView.h"
#include "./common/log.h"
#include "./common/TrayIcon.h"
#include "ConfigMng.h"

// CAngoDlg 对话框
class CAngoDlg : public CDialogEx
{
// 构造
public:
	CAngoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ANGO_MAIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON						m_hIcon;

	//// 
	virtual BOOL				OnInitDialog();
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void				OnClose();
	afx_msg void				OnPaint();
	afx_msg HCURSOR				OnQueryDragIcon();

	///// 响应菜单
	afx_msg void				OnToolCustom();
	afx_msg void				OnConfigCustom();
	afx_msg void				OnMenuAbout();
	afx_msg void				OnMenuExit();

	afx_msg void				OnMenuUp();
	afx_msg void				OnMenuDown();
	afx_msg void				OnMenuShow();
	afx_msg void				OnMenuHide();
	
	
	
	//// 响应按键
	virtual void				OnOK();							//重载
	virtual void				OnCancel();						//重载
	afx_msg void				OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	

	//// 自定义消息
	afx_msg LRESULT				OnNotifyIcon(WPARAM, LPARAM);
	virtual void				WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	DECLARE_MESSAGE_MAP()



public:
	CCustomView					m_cusView;				//个性化界面
	CTrayIcon					m_trayIcon;				//通知区域图标
	CConfigMng*					m_pCfgMng;				//设置管理
	
	BOOL						RegAutoStart();
	CMenu						m_popMenu;

	HANDLE						m_hOnlyMutex;

	afx_msg void				OnAngoTime();			//运行AngoTime
	
};
