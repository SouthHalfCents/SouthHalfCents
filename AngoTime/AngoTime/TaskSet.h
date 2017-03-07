#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// CTaskSet 对话框

class CTaskSet : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskSet)

public:
	CTaskSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskSet();

// 对话框数据
	enum { IDD = IDD_DLG_TASK_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	CBrush					m_MyBrush;
	HBRUSH					m_MyHBrush;
	afx_msg HBRUSH			OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL			OnEraseBkgnd(CDC* pDC);

public:
	CButton					m_btnOK;
	afx_msg void			OnBnClickedOk();
	afx_msg void			OnBnClickedCancel();
	CComboBox				m_cmbTask;
	CComboBox				m_cmbHour;
	CComboBox				m_cmbMin;
	CComboBox				m_cmbFrequen;
	CDateTimeCtrl			m_dateOnce;
	CButton					m_chkMonday;
	CButton					m_chkTuesday;
	CButton					m_chkWednesday;
	CButton					m_chkThursday;
	CButton					m_chkFriday;
	CButton					m_chkSaturday;
	CButton					m_chkSunday;
};
