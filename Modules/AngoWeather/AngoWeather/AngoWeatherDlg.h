
// AngoWeatherDlg.h : 头文件
//

#pragma once

#include "ParseJson.h"
#include "ServiceThread.h"
#include "HttpWeather.h"
#include "ImageEx.h"
#include "OptionDlg.h"
#include "common/TrayIcon.h"
#include "common/Utils.h"

struct tagWeather
{
	int			m_nTp;
	int			m_nWt;
	int			m_nWd;
	int			m_nWp;
	int			m_nPs;
	int			m_nVb;
	int			m_nHd;
	TCHAR		szCity[64];
};

// CAngoWeatherDlg 对话框
class CAngoWeatherDlg : public CDialogEx,public CServiceThread
{
// 构造
public:
	CAngoWeatherDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CAngoWeatherDlg();
// 对话框数据
	enum { IDD = IDD_ANGOWEATHER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	HICON						m_hIcon;
	virtual BOOL				OnInitDialog();
	afx_msg void				OnPaint();
	afx_msg HCURSOR				OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	CParseJson*					m_pParseJson;
	CHttpWeather				m_HttpWeather;
	tagWeather					m_Weather;
	COptionDlg					m_OptionDlg;

	CImageEx					m_pImageBack;
	CImageEx					m_ImageWeather;

	CString						m_strProvince;
	CString						m_strCity;
	CString						m_strDistrict;

	bool						m_bLoadFinished;

public:
	virtual bool				OnEventThreadRun();

	int							GetValue(CString strData);
	void						DrawText(CDC*pDC, LPCTSTR lpszText, int nPosX, int nPosY, int nFlag = DT_RIGHT, int nSize = 17, int Weight = FW_MEDIUM, COLORREF colText = RGB(255, 255, 255));

	//更新窗口
	void						UpdateLayered();
	afx_msg void				OnDestroy();
	afx_msg void				OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnRButtonDown(UINT nFlags, CPoint point);
	

	virtual void				OnOK();							//重载
	virtual void				OnCancel();						//重载
	afx_msg LRESULT				OnNotifyIcon(WPARAM, LPARAM);
	virtual void				WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);


public:

	CTrayIcon					m_trayIcon;					//通知区域图标
	CMenu						m_popMenu;
	HANDLE						m_hOnlyMutex;
	
	afx_msg void				OnClose();
	afx_msg void				OnMenuShow();
	afx_msg void				OnMenuHide();
	afx_msg void				OnMenuUp();
	afx_msg void				OnMenuDown();
	afx_msg void				OnMenuCityOpt();
	afx_msg void				OnMenuPreview();
	afx_msg void				OnMenuAngo();
	afx_msg void				OnMenuConfig();
	afx_msg void				OnMenuExit();
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct);
	void						InitPosition();
};
