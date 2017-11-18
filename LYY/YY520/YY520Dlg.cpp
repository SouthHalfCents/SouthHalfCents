
// YY520Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "YY520.h"
#include "YY520Dlg.h"
#include "afxdialogex.h"
#include <atlconv.h>
#include <string>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CYY520Dlg 对话框



CYY520Dlg::CYY520Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CYY520Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCfgMng	=	NULL;
	m_popMenu.LoadMenu(IDR_MENU1);	

	m_hOnlyMutex	=	NULL;
}

void CYY520Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CYY520Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_CLOSE()

	ON_MESSAGE(MAIN_WM_NOTIFYICON, &CYY520Dlg::OnNotifyIcon)   
	ON_COMMAND(ID_MENU_ABOUT, &CYY520Dlg::OnMenuAbout)
	ON_COMMAND(ID_MENU_EXIT, &CYY520Dlg::OnMenuExit)
	ON_COMMAND(ID_CONFIG_CUSTOM, &CYY520Dlg::OnConfigCustom)
	ON_COMMAND(ID_TOOL_CUSTOM, &CYY520Dlg::OnToolCustom)
	ON_COMMAND(ID_MENU_SHOW, &CYY520Dlg::OnMenuShow)
	ON_COMMAND(ID_MENU_HIDE, &CYY520Dlg::OnMenuHide)
	ON_COMMAND(ID_MENU_UP, &CYY520Dlg::OnMenuUp)
	ON_COMMAND(ID_MENU_DOWN, &CYY520Dlg::OnMenuDown)
	ON_COMMAND(ID_ANGO_TIME, &CYY520Dlg::OnAngoTime)
	ON_COMMAND(ID_MENU_SNOW, &CYY520Dlg::OnSnow)
END_MESSAGE_MAP()


// CYY520Dlg 消息处理程序
void CYY520Dlg::OnClose()
{
	//StopWork();

	if (m_hOnlyMutex)
	{
		CloseHandle(m_hOnlyMutex);
		m_hOnlyMutex = NULL;
	}
	CDialogEx::OnClose();

}
//-------------------------------------------------------------------------------------------------------------------------
BOOL CYY520Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	
	//隐藏任务栏
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// 唯一实例
	m_hOnlyMutex = CreateMutex(NULL, FALSE, _T("//YY520.exe"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 如果程序已经存在并且正在运行  如果已有互斥量存在则释放句柄并复位互斥量，退出程序
		CloseHandle(m_hOnlyMutex);
		m_hOnlyMutex = NULL;
		//AngoBox.AngoMessageBox(_T("程序已经在运行"));
		CDialog::OnCancel();
		return TRUE;
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

void CYY520Dlg::OnPaint()
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
HCURSOR CYY520Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//重载windows帮助，屏蔽F1帮助
void CYY520Dlg::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	return;
	//CDialog::WinHelp(dwData, nCmd);
}

int CYY520Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	//通知区域图标，托盘图标
	m_trayIcon.m_pCwnd		=	this;
	m_trayIcon.m_dwIconId	=	IDR_MAINFRAME;
	m_trayIcon.m_strTips	=	"爱你（づ￣3￣）づ";
	m_trayIcon.InitTrayIcon();


	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------
//拖动无标题对话框窗口
void CYY520Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

//拖动无标题对话框窗口
void CYY520Dlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu* pmenu = m_popMenu.GetSubMenu(0);		//弹出的菜单实际上是IDR_MENU_POPUP菜单的某项的子菜单，这里是第一项
	CPoint pos;
	GetCursorPos(&pos);							//弹出菜单的位置，这里就是鼠标的当前位置
	//显示该菜单，第一个参数的两个值分别表示在鼠标的右边显示、响应鼠标右击
	pmenu->TrackPopupMenu( TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CYY520Dlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
		if (AngoBox.AngoMessageBox(strMsg, L"退出", MB_OKCANCEL) == IDOK)
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
		AngoBox.AngoMessageBox(strMsg,L"按键", MB_POST_CENTER);
	}


	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CYY520Dlg::OnOK()
{
	CString strMsg = L"回车键";
	AngoBox.AngoMessageBox(strMsg);

}

void CYY520Dlg::OnCancel()
{
	CString strMsg;
	strMsg = "是否退出程序？";
	//if( AngoBox.AngoMessageBox(strMsg,L"退出",MB_OKCANCEL) == IDCANCEL)
	CDialog::OnCancel();
}

//-------------------------------------------------------------------------------------------------------------------------
LRESULT CYY520Dlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
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




void CYY520Dlg::OnMenuAbout()
{
	CString strMsg;
	strMsg = "说明";
	AngoBox.AngoMessageBox(strMsg);
}

void CYY520Dlg::OnMenuExit()
{
	OnClose();
	OnCancel();
}


void CYY520Dlg::OnConfigCustom()
{
	m_pCfgMng = new CConfigMng;
	m_pCfgMng->DoModal();
	delete m_pCfgMng;
	m_pCfgMng = NULL;
}
//-------------------------------------------------------------------------------------------------------------------------



//设置自启动
BOOL CYY520Dlg::RegAutoStart()
{
	CString strKeyName = _T("YY520");
	CString strKeyValue = CUtils::GetAppPath();

	BOOL bResult = CUtils::SetRegAutoStart(TRUE, strKeyName, strKeyValue);
	return bResult;

}
//-------------------------------------------------------------------------------------------------------------------------




void CYY520Dlg::OnToolCustom()
{
	AngoBox.AngoMessageBox(_T("工具箱"));
}

void CYY520Dlg::OnMenuShow()
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

void CYY520Dlg::OnMenuHide()
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

void CYY520Dlg::OnMenuUp()
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

void CYY520Dlg::OnMenuDown()
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


void CYY520Dlg::OnAngoTime()
{

	CString strPath = CUtils::GetAppDir();
	strPath += _T("\\YY520Time.exe");

	USES_CONVERSION;
#ifdef  UNICODE                     
	WinExec(T2A(strPath), SW_SHOW);
#else           
	WinExec(strPath, SW_SHOW);
#endif   

	return;

	//CString strPath = CUtils::GetAppDir();
	//strPath += _T("\\AngoTime.exe");
	//这个API会提示是否以管理员身份启动
	//ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL);

}

void CYY520Dlg::OnSnow()
{
	CString strPath = CUtils::GetAppDir();
	strPath += _T("\\Snow.exe");

	USES_CONVERSION;
#ifdef  UNICODE                     
	WinExec(T2A(strPath), SW_SHOW);
#else           
	WinExec(strPath, SW_SHOW);
#endif   


}
