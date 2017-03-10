// Clock.cpp : 实现文件
//

#include "stdafx.h"
#include "AngoTime.h"
#include "Clock.h"
#include "afxdialogex.h"
#include "Database.h"
#include "Utils.h"

// CClock 对话框

IMPLEMENT_DYNAMIC(CClock, CDialogEx)



CClock::CClock(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClock::IDD, pParent)
{
	m_MyBrush.CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
	m_MyHBrush = CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
}

CClock::~CClock()
{
	if (m_MyBrush.GetSafeHandle())
		m_MyBrush.DeleteObject();
	DeleteObject(m_MyHBrush);
}

void CClock::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLOCK, m_lstContent);
	DDX_Control(pDX, IDC_EDIT_MUSIC_PATH, m_edtPath);
	DDX_Control(pDX, IDC_COMBO_HOUR, m_cmbHour);
	DDX_Control(pDX, IDC_COMBO_MIN, m_cmbMin);
}


BEGIN_MESSAGE_MAP(CClock, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CClock::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_MODI, &CClock::OnBnClickedButtonModi)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CClock::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_CHOOCE, &CClock::OnBnClickedButtonChooce)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CClock::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &CClock::OnBnClickedButtonCancle)
END_MESSAGE_MAP()


// CClock 消息处理程序
HBRUSH CClock::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CClock::OnEraseBkgnd(CDC* pDC)
{
	CBrush *pOldBrush = pDC->SelectObject(&m_MyBrush);

	CRect Rect;
	GetClientRect(&Rect);

	pDC->FillRect(&Rect, &m_MyBrush);
	pDC->SelectObject(pOldBrush);
	return TRUE;

}


int CClock::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

BOOL CClock::OnInitDialog()
{
	CDialog::OnInitDialog();

	//居中
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	int xSize = ::GetSystemMetrics(SM_CXSCREEN);
	int ySize = ::GetSystemMetrics(SM_CYSCREEN);
	int Width = rcWindow.Width();
	int Height = rcWindow.Height();

	rcWindow.left = (xSize - Width) / 2;
	rcWindow.right = rcWindow.left + Width;
	rcWindow.top = (ySize - Height) / 2;
	rcWindow.bottom = rcWindow.top + Height;

	MoveWindow(&rcWindow);

	m_lstContent.InsertColumn(0, L"响铃时间", LVCFMT_LEFT, 120, 0);
	m_lstContent.InsertColumn(1, L"重复", LVCFMT_LEFT, 120, 1);
	m_lstContent.InsertColumn(2, L"说明", LVCFMT_LEFT, 100, 2);
	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------
list<CLOCK_CONT>	CClock::m_ClcokList;
void CClock::CheckClock(CTime & tTime)
{
	list<CLOCK_CONT>::iterator clock_itr;
	BOOL bFind = FALSE;
	CLOCK_CONT clock_alarm = { 0 };

	for (clock_itr = m_ClcokList.begin(); clock_itr != m_ClcokList.end(); )
	{
		bFind = FALSE;
		switch (clock_itr->nFreq)
		{
		case TIMES_ONCE:
		case TIMES_DAY:
		{
			if (tTime.GetYear() == clock_itr->tTime.GetYear() && tTime.GetMonth() == clock_itr->tTime.GetMonth() && tTime.GetDay() == clock_itr->tTime.GetDay())
			{
				if (tTime.GetHour() == clock_itr->tTime.GetHour() && tTime.GetMinute() == clock_itr->tTime.GetMinute())
				{
					bFind = TRUE;
				}
			}
		}
		break;

		case TIMES_WEEK:
		{
			int nDay = 0;
			CUtils::TranDayweekToInt(tTime.GetDayOfWeek(), nDay);
			if (nDay | clock_itr->nWeek)
			{
				if (tTime.GetYear() == clock_itr->tTime.GetYear() && tTime.GetMonth() == clock_itr->tTime.GetMonth() && tTime.GetDay() == clock_itr->tTime.GetDay())
				{
					if (tTime.GetHour() == clock_itr->tTime.GetHour() && tTime.GetMinute() == clock_itr->tTime.GetMinute())
					{
						bFind = TRUE;
					}
				}
			}
		}
		break;


		default:
			break;
		}

		//下一个
		if (!bFind)
		{
			++clock_itr;
			continue;
		}
			
		if (TIMES_ONCE == clock_itr->nFreq)
		{
			m_ClcokList.erase(clock_itr++);
		}
		else
		{
			++clock_itr;
		}


		//任务线程 信号+1
		ReleaseSemaphore(g_hSemaph_Alarm, 1, NULL);
	}


}

void CClock::OnBnClickedButtonAdd()
{
	CClockSet clockSetDlg;
	if (IDOK == clockSetDlg.DoModal())
	{

	}
}


void CClock::OnBnClickedButtonModi()
{
	CClockSet clockSetDlg;
	if (IDOK == clockSetDlg.DoModal())
	{

	}
}


void CClock::OnBnClickedButtonDel()
{
	if (IDOK == AngoMessageBox(_T("删除")) )
	{

	}
}


void CClock::OnBnClickedButtonChooce()
{
	CFileDialog dlg(TRUE, _T("mp3"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,_T("mp3文件(*.mp3)|*.mp3|wav文件(*.wav)|*.wav|所有文件(*.*) |*.*||"));

	CString strInitDir = CUtils::GetAppDir();
	strInitDir += _T("\\music");

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
	dlg.m_ofn.lpstrTitle = _T("选择闹铃文件");
	INT_PTR nRet = dlg.DoModal();
	if (nRet != IDOK)
		return;

	m_edtPath.SetWindowTextW(dlg.GetPathName());

}


void CClock::OnBnClickedButtonOk()
{
	EndDialog(IDOK);
}


void CClock::OnBnClickedButtonCancle()
{
	EndDialog(IDCANCEL);
}
