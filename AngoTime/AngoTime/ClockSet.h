#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "resource.h"

// CClockSet �Ի���

class CClockSet : public CDialogEx
{
	DECLARE_DYNAMIC(CClockSet)

public:
	CClockSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClockSet();

// �Ի�������
	enum { IDD = IDD_DLG_CLOCK_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	CBrush					m_MyBrush;
	HBRUSH					m_MyHBrush;
	afx_msg HBRUSH			OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL			OnEraseBkgnd(CDC* pDC);

public:

	CEdit					m_edtClockName;
	CComboBox				m_cmbHour;
	CComboBox				m_cmbMin;
	CComboBox				m_cmbFrequen;
	CDateTimeCtrl			m_dateOnce;
	CButton					m_chkMonday;
	CButton					m_chkTuesday;
	CButton					m_chkWednesday;
	CButton					m_btnThursday;
	CButton					m_Friday;
	CButton					m_chkSaturday;
	CButton					m_chkSunday;
	CButton					m_btnOK;
	afx_msg void			OnBnClickedOk();
	afx_msg void			OnBnClickedCancel();
};
