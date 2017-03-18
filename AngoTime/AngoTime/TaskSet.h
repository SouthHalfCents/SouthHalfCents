#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "Database.h"

//json
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"


//��������
#define TASK_TYPE_CLOSE		0		//��ʱ�ػ�
#define TASK_TYPE_SCRIPT	1		//ִ�нű�




static num_string_t g_TaskType[] =
{
	{ TASK_TYPE_CLOSE, _T("��ʱ�ػ�") },
	{ TASK_TYPE_SCRIPT, _T("ִ�нű�") }
};


// CTaskSet �Ի���

class CTaskSet : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskSet)

public:
	CTaskSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTaskSet();

// �Ի�������
	enum { IDD = IDD_DLG_TASK_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
	afx_msg void			OnBnClickedBtnScript();			//ѡ��ű��ļ�
	CEdit m_edtScript;
	afx_msg void OnCbnSelchangeComboTask();
	CButton m_btnScript;
};
