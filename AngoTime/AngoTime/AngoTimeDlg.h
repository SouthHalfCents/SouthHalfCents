
// AngoTimeDlg.h : ͷ�ļ�
//

#pragma once
#include "TrayIcon.h"

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
		int x;
		int y;
		int z;
	};

public:
	
	CTrayIcon					m_trayIcon;				//֪ͨ����ͼ��
	CMenu						m_popMenu;


	UINT_PTR					m_uClock_Timer;				// ʱ�Ӵ�����
	CBrush						m_cBrush;
	CPoint						m_Point_Start;
	CPoint						m_Point_End;
	int							m_s;
	float						m_m;
	float						m_h;
	afx_msg HBRUSH				OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void				OnTimer(UINT_PTR nIDEvent);
	void						InitClock();					// ��ʼ��
	void						ClockTime();					// ִ��ʱ�Ӳ���
	void						GetRgb(CPoint &cPoint, tagRGB &tRgb);			// ����λ�û�ȡRGBֵ
	void						GetRgb(int &nPos, tagRGB &tRgb);				// ����ʱ��λ��(x/60)��ȡRGBֵ



	afx_msg void				OnMenuExit();					//�˳�
	afx_msg void				OnViewUp();						//��ǰ
	afx_msg void				OnViewDown();					//ȡ����ǰ
	afx_msg void				OnViewShow();					//��ʾ
	afx_msg void				OnViewHide();					//����
	afx_msg void				OnMenuAngo();					//����Ango������
};
