
// AngoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Ango.h"
#include "AngoDlg.h"
#include "MsgBoxEx.h"
#include "afxdialogex.h"

#include <string>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAngoDlg 对话框



CAngoDlg::CAngoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCfgMng	=	NULL;
	m_popMenu.LoadMenu(IDR_MENU1);	
}

void CAngoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAngoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()

	ON_MESSAGE(MAIN_WM_NOTIFYICON, &CAngoDlg::OnNotifyIcon)   
	ON_COMMAND(ID_MENU_ABOUT, &CAngoDlg::OnMenuAbout)
	ON_COMMAND(ID_MENU_EXIT, &CAngoDlg::OnMenuExit)
	ON_COMMAND(ID_CONFIG_CUSTOM, &CAngoDlg::OnConfigCustom)
	ON_COMMAND(ID_TOOL_CUSTOM, &CAngoDlg::OnToolCustom)
	ON_COMMAND(ID_MENU_SHOW, &CAngoDlg::OnMenuShow)
	ON_COMMAND(ID_MENU_HIDE, &CAngoDlg::OnMenuHide)
	ON_COMMAND(ID_MENU_UP, &CAngoDlg::OnMenuUp)
	ON_COMMAND(ID_MENU_DOWN, &CAngoDlg::OnMenuDown)
	ON_COMMAND(ID_ANGO_TIME, &CAngoDlg::OnAngoTime)
END_MESSAGE_MAP()


// CAngoDlg 消息处理程序
//-------------------------------------------------------------------------------------------------------------------------
BOOL CAngoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	
	//隐藏任务栏
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// 唯一实例
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, L"//Ango.exe");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 如果程序已经存在并且正在运行 
		// 如果已有互斥量存在则释放句柄并复位互斥量，退出程序
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		AngoMessageBox(_T("程序已经在运行"));
		CDialog::OnCancel();
	}



	//自定义对话框界面
	vector<DWORD> vData;
	vData.push_back(IDB_BITMAP_MAIN_1);
	vData.push_back(IDB_BITMAP_MAIN_2);
	m_cusView.m_pCwnd = this;
	m_cusView.InitBmpVector(vData);
	m_cusView.InitCusView();


	//调整位置
	CRect cr;
	GetClientRect(&cr);//获取对话框客户区域大小
	ClientToScreen(&cr);//转换为荧幕坐标
	int x = GetSystemMetrics(SM_CXSCREEN);//获取荧幕坐标的宽度，单位为像素
	int y = GetSystemMetrics(SM_CYSCREEN);//获取荧幕坐标的高度，单位为像素
	//MoveWindow((x-cr.Width() *2)/2 ,cr.top,cr.Width() *2,cr.Height() *2);//左上角
	MoveWindow(x-cr.Width()+150 ,y-cr.Height()+150,cr.Width(),cr.Height());
	//MoveWindow(x-cr.Width() ,y-cr.Height(),cr.Width(),cr.Height());

	OnMenuShow();
	OnMenuUp();
	//RegAutoStart();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAngoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{	
		//自定义对话框界面
		m_cusView.OnPaint();
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CAngoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//重载windows帮助，屏蔽F1帮助
void CAngoDlg::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	return;
	//CDialog::WinHelp(dwData, nCmd);
}

int CAngoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	//通知区域图标，托盘图标
	m_trayIcon.m_pCwnd		=	this;
	m_trayIcon.m_dwIconId	=	IDR_MAINFRAME;
	m_trayIcon.m_strTips	=	"╮(￣▽￣)╭ ";
	m_trayIcon.InitTrayIcon();


	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------
//拖动无标题对话框窗口
void CAngoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

//拖动无标题对话框窗口
void CAngoDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu* pmenu = m_popMenu.GetSubMenu(0);		//弹出的菜单实际上是IDR_MENU_POPUP菜单的某项的子菜单，这里是第一项
	CPoint pos;
	GetCursorPos(&pos);							//弹出菜单的位置，这里就是鼠标的当前位置
	//显示该菜单，第一个参数的两个值分别表示在鼠标的右边显示、响应鼠标右击
	pmenu->TrackPopupMenu( TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CAngoDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString strMsg;
	switch (nChar)
	{

	case 13:
	{
		strMsg = "回车键";
	}
	break;

	case 16:
	{
		m_cusView.SetNextView();
		OnPaint();
		//MessageBox(L"Shift键");
	}
	break;

	case 27:
	{
		strMsg = "是否退出程序？";
		if (AngoMessageBox(strMsg, L"退出", MB_OKCANCEL) == IDOK)
		{
			CDialog::OnCancel();
		}
	}
	break;

	case 112:
	{
		strMsg = "F1键";
	}
	break;

	case 113:
	{
		strMsg = "F2键";
	}
	break;

	case 114:
	{
		strMsg = "F3键";
	}
	break;

	case 115:
	{
		strMsg = "F4键";
	}
	break;

	case 116:
	{
		strMsg = "F5键";
	}
	break;

	case 117:
	{
		strMsg = "F6键";
	}
	break;
	default:
		strMsg = "其他键";
	}

	if (strMsg.GetLength())
	{
		AngoMessageBox(strMsg,L"按键", MB_POST_CENTER);
	}


	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CAngoDlg::OnOK()
{
	CString strMsg = L"回车键";
	AngoMessageBox(strMsg);

}

void CAngoDlg::OnCancel()
{
	CString strMsg;
	strMsg = "是否退出程序？";
	//if( AngoMessageBox(strMsg,L"退出",MB_OKCANCEL) == IDCANCEL)
	CDialog::OnCancel();
}

//-------------------------------------------------------------------------------------------------------------------------
LRESULT CAngoDlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
{
	if (lparam == WM_LBUTTONDOWN)
	{
		//恢复窗口或者最小化
		//AfxGetMainWnd()->ShowWindow(SW_SHOW);
		//AfxGetMainWnd()->ShowWindow(SW_RESTORE);
		//这里貌似只有写这样两句才能保证恢复窗口后，该窗口处于活动状态（在最前面）
	}
	else if (lparam == WM_RBUTTONDOWN)
	{
		//弹出右键菜单
		CMenu* pmenu = m_popMenu.GetSubMenu(0);		//弹出的菜单实际上是IDR_MENU_POPUP菜单的某项的子菜单，这里是第一项
		CPoint pos;
		GetCursorPos(&pos);							//弹出菜单的位置，这里就是鼠标的当前位置
		//显示该菜单，第一个参数的两个值分别表示在鼠标的右边显示、响应鼠标右击
		pmenu->TrackPopupMenu( TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------------




void CAngoDlg::OnMenuAbout()
{
	CString strMsg;
	strMsg = "说明";
	AngoMessageBox(strMsg);
}

void CAngoDlg::OnMenuExit()
{
	OnCancel();
}


void CAngoDlg::OnConfigCustom()
{
	m_pCfgMng = new CConfigMng;
	m_pCfgMng->DoModal();
	delete m_pCfgMng;
	m_pCfgMng = NULL;
}
//-------------------------------------------------------------------------------------------------------------------------



//设置自启动
BOOL CAngoDlg::RegAutoStart()
{
	CString strKeyName = _T("Ango");
	CString strKeyValue = CUtils::GetAppPath();

	BOOL bResult = CUtils::SetRegAutoStart(TRUE, strKeyName, strKeyValue);
	return bResult;

}
//-------------------------------------------------------------------------------------------------------------------------




void CAngoDlg::OnToolCustom()
{
	AngoMessageBox(L"工具箱");
}

void CAngoDlg::OnMenuShow()
{
	//这里貌似只有写这样两句才能保证恢复窗口后，该窗口处于活动状态（在最前面）
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);

	CMenu* pMenu = m_popMenu.GetSubMenu(0);	
	//设置第一层：工具箱勾选
	//pmenu->CheckMenuItem(ID_MENU_TOOL, MF_BYCOMMAND | MF_CHECKED);	//通过命令ID，选中ID_MENU_TOOL
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(0);
		if (pMenu)
		{
			pMenu->CheckMenuItem(ID_MENU_SHOW, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(ID_MENU_HIDE, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}
}

void CAngoDlg::OnMenuHide()
{
	//AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
	/*这个对话框不显示在任务栏，在最小化之后它会变成一个很小的一条显示在桌面上。这时如果它被遮挡，就会出发WM_PAINT消息*/

	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(0);
		if (pMenu)
		{
			pMenu->CheckMenuItem(ID_MENU_SHOW, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_MENU_HIDE, MF_BYCOMMAND | MF_CHECKED);
		}
	}
}

void CAngoDlg::OnMenuUp()
{
	CRect rtClient;
	GetWindowRect(rtClient);
	::SetWindowPos(m_hWnd, HWND_TOPMOST, rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), SWP_SHOWWINDOW);

	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(0);
		if (pMenu)
		{
			pMenu->CheckMenuItem(ID_MENU_UP, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(ID_MENU_DOWN, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}
}

void CAngoDlg::OnMenuDown()
{
	CRect rtClient;
	GetWindowRect(rtClient);
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), SWP_SHOWWINDOW);

	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(0);
		if (pMenu)
		{
			pMenu->CheckMenuItem(ID_MENU_UP, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_MENU_DOWN, MF_BYCOMMAND | MF_CHECKED);
		}
	}
}


void CAngoDlg::OnAngoTime()
{

	CString strPath = CUtils::GetAppDir();
	strPath += _T("\\AngoTime.exe");
	//这个API会提示是否以管理员身份启动
	ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL);

}
