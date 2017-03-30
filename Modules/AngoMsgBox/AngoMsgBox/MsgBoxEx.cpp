// MsgBoxEx.cpp : 实现文件
//

#include "stdafx.h"
#include "AngoMsgBox.h"
#include "MsgBoxEx.h"
#include "afxdialogex.h"


// CMsgBoxEx 对话框

IMPLEMENT_DYNAMIC(CMsgBoxEx, CDialogEx)

CMsgBoxEx::CMsgBoxEx(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMsgBoxEx::IDD, pParent)
{
	m_strYes = "";
	m_strNo = "";
	m_strCancle = "";
	m_strMsg = "";
	m_strTitle = "";
	m_nType = 0;

	m_MyBrush.CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
	m_MyHBrush = CreateSolidBrush(DIALOG_BACKGROUND_COLOR);
}

CMsgBoxEx::~CMsgBoxEx()
{
	if (m_MyBrush.GetSafeHandle())
		m_MyBrush.DeleteObject();
	DeleteObject(m_MyHBrush);
}

void CMsgBoxEx::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_YES, m_btnYes);
	DDX_Control(pDX, IDC_BTN_NO, m_btnNo);
	DDX_Control(pDX, IDC_BTN_CANCLE, m_btnCancle);
	DDX_Control(pDX, IDC_EDIT_MSG, m_edtMsg);
}


BEGIN_MESSAGE_MAP(CMsgBoxEx, CDialogEx)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()

	ON_BN_CLICKED(IDC_BTN_YES, &CMsgBoxEx::OnBnClickedBtnYes)
	ON_BN_CLICKED(IDC_BTN_NO, &CMsgBoxEx::OnBnClickedBtnNo)
	ON_BN_CLICKED(IDC_BTN_CANCLE, &CMsgBoxEx::OnBnClickedBtnCancle)
END_MESSAGE_MAP()


// CMsgBoxEx 消息处理程序
HBRUSH CMsgBoxEx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CMsgBoxEx::OnEraseBkgnd(CDC* pDC)
{
	CBrush *pOldBrush = pDC->SelectObject(&m_MyBrush);

	CRect Rect;
	GetClientRect(&Rect);

	pDC->FillRect(&Rect, &m_MyBrush);
	pDC->SelectObject(pOldBrush);
	return TRUE;

}


void CMsgBoxEx::OnBnClickedBtnYes()
{
	DWORD dwEndcode = IDCANCEL;
	switch (m_nType)
	{
	case MB_OK:
		break;

	case MB_OKCANCEL:
		break;

	case MB_ABORTRETRYIGNORE:
	{
		//m_strYes = "终止";
		dwEndcode = IDABORT;
	}
	break;

	case MB_YESNOCANCEL:
	{
		//m_strYes = "是";
		dwEndcode = IDYES;
	}
	break;

	case MB_YESNO:
		break;


	case MB_RETRYCANCEL:
		break;

	case MB_CANCELTRYCONTINUE:
	{
		//m_strYes = "取消";
		dwEndcode = IDCANCEL;
	}
	break;

	default:
		break;
	}



	EndDialog(dwEndcode);
}


void CMsgBoxEx::OnBnClickedBtnNo()
{
	DWORD dwEndcode = IDCANCEL;

	switch (m_nType)
	{
	case MB_OK:
	{
		//m_strNo = "确定";
		dwEndcode = IDOK;
	}
	break;

	case MB_OKCANCEL:
	{
		//m_strNo = "确定";
		dwEndcode = IDOK;
	}
	break;

	case MB_ABORTRETRYIGNORE:
	{
		//m_strNo = "重试";
		dwEndcode = IDRETRY;
	}
	break;

	case MB_YESNOCANCEL:
	{
		//m_strNo = "否";
		dwEndcode = IDNO;
	}
	break;

	case MB_YESNO:
	{
		//m_strNo = "是";
		dwEndcode = IDYES;
	}
	break;

	case MB_RETRYCANCEL:
	{
		//m_strNo = "重试";
		dwEndcode = IDRETRY;
	}
	break;

	case MB_CANCELTRYCONTINUE:
	{
		//m_strNo = "重试";
		dwEndcode = IDRETRY;
	}
	break;

	default:
		break;
	}

	EndDialog(dwEndcode);
}


void CMsgBoxEx::OnBnClickedBtnCancle()
{
	DWORD dwEndcode = IDCANCEL;

	switch (m_nType)
	{
	case MB_OK:
		break;

	case MB_OKCANCEL:
	{
		//m_strCancle = "取消";
		dwEndcode = IDCANCEL;
	}
	break;

	case MB_ABORTRETRYIGNORE:
	{
		//m_strCancle = "取消";
		dwEndcode = IDCANCEL;
	}
	break;

	case MB_YESNOCANCEL:
	{
		//m_strCancle = "取消";
		dwEndcode = IDCANCEL;
	}
	break;

	case MB_YESNO:
	{
		//m_strCancle = "否";
		dwEndcode = IDNO;
	}
	break;

	case MB_RETRYCANCEL:
	{
		//m_strCancle = "取消";
		dwEndcode = IDCANCEL;
	}
	break;

	case MB_CANCELTRYCONTINUE:
	{
		//m_strNo = "继续";
		dwEndcode = IDCONTINUE;
	}
	break;

	default:
		break;
	}

	EndDialog(dwEndcode);
}


void CMsgBoxEx::InitMsgBox(CString strMsg, CString strTitle/* =L"" */, UINT nType/* =MB_OK */)
{
	m_strMsg = strMsg;
	m_strTitle = strTitle;
	m_nType = nType;
}



int CMsgBoxEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CMsgBoxEx::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetRayout();

	//居中
	if (m_nType & MB_POST_CENTER)
	{
		SetDlgPos();
	}


	return TRUE;
}

void CMsgBoxEx::SetRayout()
{

	m_Font.CreatePointFont(120, _T("新宋体"));
	m_edtMsg.SetFont(&m_Font);		//设置字体
	m_edtMsg.SetWindowText(m_strMsg);



	this->SetWindowText(m_strTitle);

	switch (m_nType)
	{
	case MB_OK:
	{
		m_btnYes.ShowWindow(SW_HIDE);
		m_btnCancle.ShowWindow(SW_HIDE);
		m_strNo = "确定";
	}
	break;

	case MB_OKCANCEL:
	{
		m_btnYes.ShowWindow(SW_HIDE);
		m_strNo = "确定";
		m_strCancle = "取消";
	}
	break;

	case MB_ABORTRETRYIGNORE:
	{
		m_strYes = "终止";
		m_strNo = "重试";
		m_strCancle = "取消";
	}
	break;

	case MB_YESNOCANCEL:
	{
		m_strYes = "是";
		m_strNo = "否";
		m_strCancle = "取消";
	}
	break;

	case MB_YESNO:
	{
		m_btnYes.ShowWindow(SW_HIDE);
		m_strNo = "是";
		m_strCancle = "否";
	}
	break;


	case MB_RETRYCANCEL:
	{
		m_btnYes.ShowWindow(SW_HIDE);
		m_strNo = "重试";
		m_strCancle = "取消";
	}
	break;

	case MB_CANCELTRYCONTINUE:
	{
		m_strYes = "取消";
		m_strNo = "重试";
		m_strCancle = "继续";
	}
	break;

	default:
	{
		m_btnYes.ShowWindow(SW_HIDE);
		m_btnCancle.ShowWindow(SW_HIDE);
		m_strNo = "确定";
	}
	break;
	}

	m_btnYes.SetWindowTextW(m_strYes);
	m_btnNo.SetWindowTextW(m_strNo);
	m_btnCancle.SetWindowTextW(m_strCancle);
}


void CMsgBoxEx::SetDlgPos()
{
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
}
//---------------------------------------------------------------------------------------------------------------------
// int AngoMessageBox(CString strMsg, CString strTitle/* =L"" */, UINT nType/* =MB_POST_BESIDE | MB_OK */)
// {
// 	AFX_MANAGE_STATE(AfxGetStaticModuleState());
// 	CMsgBoxEx * pMsgBox = new CMsgBoxEx;
// 	pMsgBox->InitMsgBox(strMsg, strTitle, nType);
// 	int nReturn = (int)pMsgBox->DoModal();
// 	delete pMsgBox;
// 
// 	return nReturn;
// }
//---------------------------------------------------------------------------------------------------------------------