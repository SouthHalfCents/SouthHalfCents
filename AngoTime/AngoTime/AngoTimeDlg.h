
// AngoTimeDlg.h : ͷ�ļ�
//

#pragma once
#include "TrayIcon.h"
#include "Clock.h"
#include "Config.h"
#include "Task.h"

#include <hash_map>
using namespace std;


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

	struct hash_PosRgb
	{
		enum
		{   //   parameters   for   hash   table     
			bucket_size = 4,   //   0   <   bucket_size     
			min_buckets = 8  //   min_buckets   =   2   ^^   N,   0   <   N     
		};

		size_t operator() (const CPoint & cPos) const
		{
			return size_t(cPos.x *10000 + cPos.y);
		}

		bool operator()(const CPoint& cPos1, const CPoint& cPos2) const
		{
			return cPos1.x == cPos2.x && cPos1.y == cPos2.y;
		}
	};



	hash_map<CPoint,COLORREF, hash_PosRgb>	m_mapPointRgb;
	void						InitRgbMap();
	afx_msg HBRUSH				OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void				OnTimer(UINT_PTR nIDEvent);
	void						InitClock();							// ��ʼ��
	HANDLE						m_hThread_Clock;
	HANDLE						m_hSemaph_Clock;


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
	afx_msg void				OnCfgOther();					//����


};


static unsigned int __stdcall	Thread_Clock(LPVOID pParam);					// ִ��ʱ�Ӳ���