// Clock.cpp : 实现文件
//

#include "stdafx.h"
#include "AngoTime.h"
#include "Clock.h"
#include "afxdialogex.h"


// CClock 对话框

IMPLEMENT_DYNAMIC(CClock, CDialogEx)

list<CLOCK_CONT> CClock::m_ClcokList;


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
}


BEGIN_MESSAGE_MAP(CClock, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
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


	return TRUE;
}