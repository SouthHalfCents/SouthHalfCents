// ClockSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YY520Time.h"
#include "ClockSet.h"
#include "afxdialogex.h"


// CClockSet �Ի���

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
	DDX_Control(pDX, IDC_EDIT_CLOCK_NAME, m_edtClockName);
	DDX_Control(pDX, IDC_CMB_HOUR, m_cmbHour);
	DDX_Control(pDX, IDC_CMB_MIN, m_cmbMin);
	DDX_Control(pDX, IDC_CMB_FREQUEN, m_cmbFrequen);
	DDX_Control(pDX, IDC_DATETIMEPICKER_ONCE, m_dateOnce);
	DDX_Control(pDX, IDC_CHK_CLOCK_MON, m_chkMonday);
	DDX_Control(pDX, IDC_CHK_CLOCK_TUES, m_chkTuesday);
	DDX_Control(pDX, IDC_CHK_CLOCK_WEDN, m_chkWednesday);
	DDX_Control(pDX, IDC_CHK_CLOCK_THUR, m_btnThursday);
	DDX_Control(pDX, IDC_CHK_CLOCK_FRI, m_Friday);
	DDX_Control(pDX, IDC_CHK_CLOCK_SAT, m_chkSaturday);
	DDX_Control(pDX, IDC_CHK_CLOCK_SUN, m_chkSunday);
	DDX_Control(pDX, IDOK, m_btnOK);
}


BEGIN_MESSAGE_MAP(CClockSet, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, &CClockSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CClockSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CClockSet ��Ϣ�������
HBRUSH CClockSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor != CTLCOLOR_EDIT && nCtlColor != CTLCOLOR_LISTBOX)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)m_MyHBrush;
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
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

void CClockSet::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	EndDialog(IDOK);
}


void CClockSet::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	EndDialog(IDCANCEL);
}
