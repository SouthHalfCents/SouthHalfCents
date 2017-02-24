#pragma once
#include "afxwin.h"


// CBaseDialog �Ի���

class CBaseDialog : public CDialog
{
	DECLARE_DYNAMIC(CBaseDialog)

public:
	CBaseDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBaseDialog();

// �Ի�������
	enum { IDD = IDD_BASE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	CBrush					m_MyBrush;
	HBRUSH					m_MyHBrush;
	afx_msg HBRUSH			OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL			OnEraseBkgnd(CDC* pDC);


};
