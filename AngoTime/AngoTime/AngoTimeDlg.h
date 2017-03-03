
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
	afx_msg void				OnClose();
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


	int							m_nSayTime;						//��ʱ����
	BOOL						m_bClockState;					//��������״̬
	BOOL						m_bAutoRun;						//��������
	void						InitSettings();					//��ȡ���ã����ö�Ӧ������ʾ

	afx_msg void				OnSaytimeNow();
	afx_msg void				OnSaytimeAll();
	afx_msg void				OnSaytimeHalf();
	afx_msg void				OnSaytimeClose();
	afx_msg void				OnCfgAutorun();					//��������

	afx_msg void				OnClockOn();
	afx_msg void				OnClockOff();
	afx_msg void				OnClockConfig();				//��������
	afx_msg void				OnMenuTask();					//��ʱ����
};


extern HANDLE				g_hThread_Alarm;
extern HANDLE				g_hThread_SayTime;
extern HANDLE				g_hThread_CusJob;

extern HANDLE				g_hSemaph_Alarm;		//����
extern HANDLE				g_hSemaph_SayTime;		//��ʱ
extern HANDLE				g_hSemaph_CusJob;		//��ʱ����

extern std::mutex			g_MutexSound;			//������
extern BOOL					g_bWork;
extern DWORD				g_dwTasktype;
unsigned int __stdcall		Thread_Alarm(LPVOID pParam);
unsigned int __stdcall		Thread_SayTime(LPVOID pParam);
unsigned int __stdcall		Thread_CusJob(LPVOID pParam);

void						InitThread();
void						CleanThread();



/*
CWinApp�����ṩ��һ�����ڶ�дӦ�ó������õķ�����
GetProfileInt
WriteProfileInt
GetProfileString
WriteProfileString
�ɷ�������ڶ�дӦ�ó������á�


SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
�ú�����Ϊ�����ᵽ�ļ�������������������,��ʱ�����WriteProfileIntд�����ݣ�����
��д�뵽����ע���λ�ã�
HKEY_CURRENT_USER\Software\Ӧ�ó��������ɵı���Ӧ�ó���\Ӧ�ó�������\
�����InitInstance�в�ִ��SetRegistryKey������WriteProfileIntд������ʱ����д�뵽
%windir%\Ӧ�ó�������.ini�С�
*/


/*
��ȡ�����ļ�
GetPrivateProfileString

*/