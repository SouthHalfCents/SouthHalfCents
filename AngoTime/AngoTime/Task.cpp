// Task.cpp : 实现文件
//

#include "stdafx.h"
#include "AngoTime.h"
#include "Task.h"
#include "afxdialogex.h"
#include "Utils.h"

// CTask 对话框

IMPLEMENT_DYNAMIC(CTask, CDialogEx)





CTask::CTask(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTask::IDD, pParent)
{
	m_MyBrush.CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
	m_MyHBrush = CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
}

CTask::~CTask()
{
	if (m_MyBrush.GetSafeHandle())
		m_MyBrush.DeleteObject();
	DeleteObject(m_MyHBrush);
}

void CTask::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TASK, m_lstContent);
}


BEGIN_MESSAGE_MAP(CTask, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CTask::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_MODI, &CTask::OnBnClickedButtonModi)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CTask::OnBnClickedButtonDel)
END_MESSAGE_MAP()


// CTask 消息处理程序
HBRUSH CTask::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CTask::OnEraseBkgnd(CDC* pDC)
{
	CBrush *pOldBrush = pDC->SelectObject(&m_MyBrush);

	CRect Rect;
	GetClientRect(&Rect);

	pDC->FillRect(&Rect, &m_MyBrush);
	pDC->SelectObject(pOldBrush);
	return TRUE;

}


int CTask::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

BOOL CTask::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitPosition();
	ReloadData();

	return TRUE;
}

BOOL CTask::InitPosition()
{
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
	return TRUE;
}

void CTask::ReloadData()
{
	m_lstContent.InsertColumn(0, L"定时任务", LVCFMT_LEFT, 120, 0);
	m_lstContent.InsertColumn(1, L"响铃时间", LVCFMT_LEFT, 120, 1);
	m_lstContent.InsertColumn(2, L"重复", LVCFMT_LEFT, 120, 2);
}

//------------------------------------------------------------------------------------------------------------------------
list<TASKCONT>	CTask::m_TaskList;

void CTask::CheckTask(CTime & tTime)
{
	list<TASKCONT>::iterator task_itr;
	BOOL bFind = FALSE;
	TASKEXE taskexe = {0};

	for (task_itr = m_TaskList.begin(); task_itr != m_TaskList.end();)
	{
		bFind = FALSE;
		switch (task_itr->nFreq)
		{
		case TIMES_ONCE:
		case TIMES_DAY:
		{
			if (tTime.GetYear()==task_itr->tTime.GetYear() && tTime.GetMonth()==task_itr->tTime.GetMonth() &&tTime.GetDay()==task_itr->tTime.GetDay())
			{
				if (tTime.GetHour()==task_itr->tTime.GetHour() && tTime.GetMinute()==task_itr->tTime.GetMinute())
				{
					bFind = TRUE;		
				}
			}
		}
		break;

		case TIMES_WEEK:
		{
			int nDay = 0;
			CUtils::TranDayweekToInt(tTime.GetDayOfWeek(),nDay);
			if (nDay | task_itr->nWeek)
			{
				if (tTime.GetYear() == task_itr->tTime.GetYear() && tTime.GetMonth() == task_itr->tTime.GetMonth() && tTime.GetDay() == task_itr->tTime.GetDay())
				{
					if (tTime.GetHour() == task_itr->tTime.GetHour() && tTime.GetMinute() == task_itr->tTime.GetMinute())
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
			 ++task_itr;
			 continue;
		}
		else if (TIMES_ONCE == task_itr->nFreq)
		{
			m_TaskList.erase(task_itr++);
		}
		else
		{
			++task_itr;
		}

		switch (task_itr->nTaskType)
		{
		case TASK_TYPE_CLOSE:
		{
			taskexe.nTaskType	= TASK_TYPE_CLOSE;
			taskexe.taskFun		= CTask::ClosePC;
			taskexe.pParameter	= NULL;
		}
		default:
			break;
		}

		//加入任务列表
		g_MutexTaskList.lock();
		g_TaskExeList.push_back(taskexe);
		g_MutexTaskList.unlock();

		//任务线程 信号+1
		ReleaseSemaphore(g_hSemaph_CusJob, 1, NULL);	
	}

	
}

//关机
void CTask::ClosePC(void *pData)
{
	system("shutdown -s -t 0");
	AngoMessageBox(_T("close pc"));
}

//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

void CTask::OnBnClickedButtonAdd()
{
	CTaskSet tasksetDlg;

	if (IDOK == tasksetDlg.DoModal())
	{
		
	}
	else
	{

	}
}


void CTask::OnBnClickedButtonModi()
{
	CTaskSet tasksetDlg;

	if (IDOK == tasksetDlg.DoModal())
	{
		
	}
	else
	{

	}
}


void CTask::OnBnClickedButtonDel()
{
	CTaskSet tasksetDlg;

	if (IDOK == AngoMessageBox(_T("删除")))
	{
		
	}
	else
	{

	}
}


