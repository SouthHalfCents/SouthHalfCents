
// AngoTimeDlg.h : ͷ�ļ�
//

#pragma once
#include "TrayIcon.h"
#include <mutex>          // std::mutex

// CAngoTimeDlg �Ի���
class CAngoTimeDlg : public CDialogEx
{
// ����
public:
	CAngoTimeDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ANGOTIME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL				OnInitDialog();
	afx_msg int					OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void				OnPaint();
	afx_msg HCURSOR				OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	//// ��Ӧ����
	virtual void				OnOK();							//����
	virtual void				OnCancel();						//����
	afx_msg void				OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnRButtonDown(UINT nFlags, CPoint point);

	//// �Զ�����Ϣ
	afx_msg LRESULT				OnNotifyIcon(WPARAM, LPARAM);
	virtual void				WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);

	struct tagRGB
	{
		DWORD r;
		DWORD g;
		DWORD b;
	};

public:
	
	CTrayIcon					m_trayIcon;				//֪ͨ����ͼ��
	CMenu						m_popMenu;


	UINT_PTR					m_uClock_Timer;				// ʱ�Ӵ�����
	CBrush						m_cBrush;
	CPoint						m_Point_Start;
	CPoint						m_Point_End;

	CPoint						m_point_lastHour;			//�ϴ�ʱ��������
	CPoint						m_point_lastMin;			//�ϴη���������
	CPoint						m_point_lastSecL;			//�ϴ����볤������
	CPoint						m_point_lastSecS;			//�ϴ������������
	BOOL						m_bFirstClock;				//��һ�ν��붨ʱ��

	COLORREF					m_cLastHour;
	COLORREF					m_cLastMin;
	COLORREF					m_cLastSecL;
	COLORREF					m_cLastSecS;

	afx_msg HBRUSH				OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void				OnTimer(UINT_PTR nIDEvent);
	void						InitClock();					// ��ʼ��
	void						ClockTime();					// ִ��ʱ�Ӳ���



	afx_msg void				OnMenuExit();					//�˳�
	afx_msg void				OnViewUp();						//��ǰ
	afx_msg void				OnViewDown();					//ȡ����ǰ
	afx_msg void				OnViewShow();					//��ʾ
	afx_msg void				OnViewHide();					//����
	afx_msg void				OnMenuAngo();					//����Ango������


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