
// AngoTimeDlg.h : 头文件
//

#pragma once
#include "TrayIcon.h"
#include <mutex>          // std::mutex

// CAngoTimeDlg 对话框
class CAngoTimeDlg : public CDialogEx
{
// 构造
public:
	CAngoTimeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ANGOTIME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL				OnInitDialog();
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void				OnPaint();
	afx_msg HCURSOR				OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	//// 响应按键
	virtual void				OnOK();							//重载
	virtual void				OnCancel();						//重载
	afx_msg void				OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnRButtonDown(UINT nFlags, CPoint point);

	//// 自定义消息
	afx_msg LRESULT				OnNotifyIcon(WPARAM, LPARAM);
	virtual void				WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);

	struct tagRGB
	{
		DWORD r;
		DWORD g;
		DWORD b;
	};

public:
	
	CTrayIcon					m_trayIcon;				//通知区域图标
	CMenu						m_popMenu;


	UINT_PTR					m_uClock_Timer;				// 时钟触发器
	CBrush						m_cBrush;
	CPoint						m_Point_Start;
	CPoint						m_Point_End;

	CPoint						m_point_lastHour;			//上次时针轴坐标
	CPoint						m_point_lastMin;			//上次分针轴坐标
	CPoint						m_point_lastSecL;			//上次秒针长轴坐标
	CPoint						m_point_lastSecS;			//上次秒针短轴坐标
	BOOL						m_bFirstClock;				//第一次进入定时器

	COLORREF					m_cLastHour;
	COLORREF					m_cLastMin;
	COLORREF					m_cLastSecL;
	COLORREF					m_cLastSecS;

	afx_msg HBRUSH				OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void				OnTimer(UINT_PTR nIDEvent);
	void						InitClock();					// 初始化
	void						ClockTime();					// 执行时钟操作



	afx_msg void				OnMenuExit();					//退出
	afx_msg void				OnViewUp();						//最前
	afx_msg void				OnViewDown();					//取消最前
	afx_msg void				OnViewShow();					//显示
	afx_msg void				OnViewHide();					//隐藏
	afx_msg void				OnMenuAngo();					//运行Ango主程序


	void						InitThread();


	afx_msg void OnClose();
	afx_msg void OnSaytimeNow();
};

extern std::mutex			g_Mutex;
extern CWinThread*			g_pthAlarm;
extern CWinThread*			g_pthSayTime;
extern CWinThread*			g_pthWork;
extern BOOL					g_bWork;


extern DWORD				g_dwTasktype;
static UINT					ThProcAlarm(LPVOID pParam);
static UINT					ThProcSayTime(LPVOID pParam);
static UINT					ThProcWork(LPVOID pParam);