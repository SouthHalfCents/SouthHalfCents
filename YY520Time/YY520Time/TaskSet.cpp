// TaskSet.cpp : 实现文件
//

#include "stdafx.h"
#include "YY520Time.h"
#include "TaskSet.h"
#include "afxdialogex.h"


// CTaskSet 对话框

IMPLEMENT_DYNAMIC(CTaskSet, CDialogEx)

CTaskSet::CTaskSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTaskSet::IDD, pParent)
{
	m_dwWeekSet = 0;
	m_MyBrush.CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
	m_MyHBrush = CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
}

CTaskSet::~CTaskSet()
{
	if (m_MyBrush.GetSafeHandle())
		m_MyBrush.DeleteObject();
	DeleteObject(m_MyHBrush);
}

void CTaskSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_COMBO_TASK, m_cmbTask);
	DDX_Control(pDX, IDC_COMBO_FREQUEN, m_cmbFrequen);
	DDX_Control(pDX, IDC_DATETIMEPICKER_ONCE, m_dateOnce);
	DDX_Control(pDX, IDC_CHECK_MON, m_chkMonday);
	DDX_Control(pDX, IDC_CHECK_TUES, m_chkTuesday);
	DDX_Control(pDX, IDC_CHECK_WEDN, m_chkWednesday);
	DDX_Control(pDX, IDC_CHECK_THUR, m_chkThursday);
	DDX_Control(pDX, IDC_CHECK_FRI, m_chkFriday);
	DDX_Control(pDX, IDC_CHECK_SAT, m_chkSaturday);
	DDX_Control(pDX, IDC_CHECK_SUN, m_chkSunday);
	DDX_Control(pDX, IDC_EDIT_SCRIPT, m_edtScript);
	DDX_Control(pDX, IDC_BTN_SCRIPT, m_btnScript);
}


BEGIN_MESSAGE_MAP(CTaskSet, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, &CTaskSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTaskSet::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_FREQUEN, &CTaskSet::OnCbnSelchangeComboFrequen)
	ON_BN_CLICKED(IDC_BTN_SCRIPT, &CTaskSet::OnBnClickedBtnScript)
	ON_CBN_SELCHANGE(IDC_COMBO_TASK, &CTaskSet::OnCbnSelchangeComboTask)
END_MESSAGE_MAP()

BOOL CTaskSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitTaskType();
	InitFrequen();

	EnableWeekChk();
	return TRUE;
}

void CTaskSet::InitTaskType()
{
	m_cmbTask.ResetContent();

	for (int i = 0; i < _countof(g_TaskType); i++)
	{
		m_cmbTask.InsertString(i, g_TaskType[i].strValue);
		m_cmbTask.SetItemData(i, g_TaskType[i].nValue);
	}
	m_cmbTask.SetCurSel(0);
}



void CTaskSet::InitFrequen()
{
	m_cmbFrequen.ResetContent();
	for (int i = 0; i < _countof(g_Clock_Freq); i++)
	{
		m_cmbFrequen.InsertString(i, g_Clock_Freq[i].strValue);
		m_cmbFrequen.SetItemData(i, g_Clock_Freq[i].nValue);
	}
	m_cmbFrequen.SetCurSel(0);
}

// CTaskSet 消息处理程序
HBRUSH CTaskSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor != CTLCOLOR_EDIT && nCtlColor != CTLCOLOR_LISTBOX)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)m_MyHBrush;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

BOOL CTaskSet::OnEraseBkgnd(CDC* pDC)
{
	CBrush *pOldBrush = pDC->SelectObject(&m_MyBrush);

	CRect Rect;
	GetClientRect(&Rect);

	pDC->FillRect(&Rect, &m_MyBrush);
	pDC->SelectObject(pOldBrush);
	return TRUE;

}

BOOL CTaskSet::IsValidData()
{
	UpdateData(TRUE);

	int nItem = m_cmbFrequen.GetCurSel();
	DWORD_PTR dwData = m_cmbFrequen.GetItemData(nItem);
	

	if (dwData == TIMES_WEEK)
	{
		m_dwWeekSet = 0;
		if (m_chkMonday.GetCheck())
			m_dwWeekSet |= DAY_MONDAY;

		if (m_chkTuesday.GetCheck())
			m_dwWeekSet |= DAY_TUESDAY;

		if (m_chkWednesday.GetCheck())
			m_dwWeekSet |= DAY_WEDNESDAY;

		if (m_chkThursday.GetCheck())
			m_dwWeekSet |= DAY_THURSDAY;

		if (m_chkFriday.GetCheck())
			m_dwWeekSet |= DAY_FRIDAY;

		if (m_chkSaturday.GetCheck())
			m_dwWeekSet |= DAY_SATURDAY;

		if (m_chkSunday.GetCheck())
			m_dwWeekSet |= DAY_SUNDAY;

		if (m_dwWeekSet == 0)
		{
			AngoBox.AngoMessageBox(L"每周执行时间至少选择一天");
			return FALSE;
		}
	}



	return TRUE;
}

void CTaskSet::OnBnClickedOk()
{
	if ( !IsValidData())
	{ 
		return;
	}
	EndDialog(IDOK);
}


void CTaskSet::OnBnClickedCancel()
{
	
	EndDialog(IDCANCEL);
}


void CTaskSet::GetTaskMsg()
{

}

void CTaskSet::SetTaskMsg()
{

}




void CTaskSet::OnCbnSelchangeComboFrequen()
{
	int nItem = m_cmbFrequen.GetCurSel();
	DWORD_PTR dwData = m_cmbFrequen.GetItemData(nItem);
	BOOL bEnable = FALSE;

	if (dwData == TIMES_WEEK)
	{
		bEnable = TRUE;
	}
	EnableWeekChk(bEnable);

}

void CTaskSet::EnableWeekChk(BOOL bEnable /* = FALSE */)
{
	m_chkMonday.EnableWindow(bEnable);
	m_chkTuesday.EnableWindow(bEnable);
	m_chkWednesday.EnableWindow(bEnable);
	m_chkThursday.EnableWindow(bEnable);
	m_chkFriday.EnableWindow(bEnable);
	m_chkSaturday.EnableWindow(bEnable);
	m_chkSunday.EnableWindow(bEnable);
}


void CTaskSet::OnBnClickedBtnScript()
{
	CFileDialog dlg(TRUE, _T("py"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, \
		_T("python文件 (*.py)|*.py|批处理文件 (*.cmd;*.bat)|*.cmd;*.bat|所有文件 (*.*)|*.*||"));

	CString strInitDir = CUtils::GetAppDir();
	strInitDir += _T("\\script");

	WCHAR szWinDir[MAX_PATH] = { 0 };
	GetCurrentDirectoryW(MAX_PATH, szWinDir);

	if (!PathIsDirectoryW(strInitDir))
	{
		if (!CreateDirectoryW(strInitDir, NULL))
		{
			GetSystemDirectoryW(szWinDir, MAX_PATH);
			strInitDir.Format(L"%c:\\", szWinDir[0]);
		}
	}


	dlg.m_ofn.lpstrInitialDir = strInitDir;
	dlg.m_ofn.lpstrTitle = _T("选择脚本文件");
	INT_PTR nRet = dlg.DoModal();

	if (nRet != IDOK)
		return;

	m_edtScript.SetWindowTextW(dlg.GetPathName());
}


void CTaskSet::OnCbnSelchangeComboTask()
{
	int nItem = m_cmbTask.GetCurSel();
	DWORD_PTR dwData = m_cmbTask.GetItemData(nItem);
	BOOL bEnable = FALSE;

	if (dwData == TASK_TYPE_SCRIPT)
	{
		bEnable = TRUE;
	}
	m_btnScript.EnableWindow(bEnable);
}
