#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "Database.h"

//json
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"


//任务类型
#define TASK_TYPE_CLOSE		0		//定时关机
#define TASK_TYPE_SCRIPT	1		//执行脚本




static num_string_t g_TaskType[] =
{
	{ TASK_TYPE_CLOSE, _T("定时关机") },
	{ TASK_TYPE_SCRIPT, _T("执行脚本") }
};


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
	virtual BOOL			OnInitDialog();
	void					InitTaskType();
	void					InitFrequen();
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
	CComboBox				m_cmbFrequen;
	CDateTimeCtrl			m_dateOnce;

	CButton					m_chkMonday;
	CButton					m_chkTuesday;
	CButton					m_chkWednesday;
	CButton					m_chkThursday;
	CButton					m_chkFriday;
	CButton					m_chkSaturday;
	CButton					m_chkSunday;
	DWORD					m_dwWeekSet;

	void					GetTaskMsg();
	void					SetTaskMsg();
	
	afx_msg void			OnCbnSelchangeComboFrequen();
	void					EnableWeekChk(BOOL bEnable = FALSE);

	BOOL					IsValidData();
	afx_msg void			OnBnClickedBtnScript();			//选择脚本文件
	CEdit m_edtScript;
	afx_msg void OnCbnSelchangeComboTask();
	CButton m_btnScript;
};
