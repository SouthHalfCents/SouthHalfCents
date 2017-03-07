// ClockSet.cpp : 实现文件
//

#include "stdafx.h"
#include "AngoTime.h"
#include "ClockSet.h"
#include "afxdialogex.h"


// CClockSet 对话框

IMPLEMENT_DYNAMIC(CClockSet, CDialogEx)

CClockSet::CClockSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClockSet::IDD, pParent)
{
	m_MyBrush.CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
	m_MyHBrush = CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
}

CClockSet::~CClockSet()
{
	if (m_MyBrush.GetSafeHandle())
		m_MyBrush.DeleteObject();
	DeleteObject(m_MyHBrush);
}

void CClockSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CClockSet, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CClockSet 消息处理程序
HBRUSH CClockSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CClockSet::OnEraseBkgnd(CDC* pDC)
{
	CBrush *pOldBrush = pDC->SelectObject(&m_MyBrush);

	CRect Rect;
	GetClientRect(&Rect);

	pDC->FillRect(&Rect, &m_MyBrush);
	pDC->SelectObject(pOldBrush);
	return TRUE;

}