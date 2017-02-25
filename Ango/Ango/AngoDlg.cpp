
// AngoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Ango.h"
#include "AngoDlg.h"
#include "MsgBoxEx.h"
#include "afxdialogex.h"

#include "string"
#include "Database.h"

#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using std::string;

// CAngoDlg 对话框



CAngoDlg::CAngoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCfgMng	=	NULL;
	m_bUplayer	=	TRUE;
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
	ON_COMMAND(ID_SHOW_DLG, &CAngoDlg::OnShowDlg)
	ON_COMMAND(ID_ABOUT_DLG, &CAngoDlg::OnAboutDlg)
	ON_COMMAND(ID_EXIT_DLG, &CAngoDlg::OnExitDlg)
	ON_COMMAND(ID_CONFIG, &CAngoDlg::OnConfig)
	ON_COMMAND(ID_UP_LAYER, &CAngoDlg::OnUpLayer)
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
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, L"Ango");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 如果程序已经存在并且正在运行 
		// 如果已有互斥量存在则释放句柄并复位互斥量，退出程序
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		MessageBox(L"程序已经在运行");
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


	CRect rtClient;
	GetWindowRect(rtClient);
	::SetWindowPos(m_hWnd, HWND_TOPMOST, rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), SWP_SHOWWINDOW);

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
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU1);				//IDR_MENU_POPUP是在ResourceView中创建并编辑的一个菜单
	CMenu* pmenu = popMenu.GetSubMenu(0);		//弹出的菜单实际上是IDR_MENU_POPUP菜单的某项的子菜单，这里是第一项
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
		if (AngoMsgBox::MessageBox(strMsg, L"退出", MB_OKCANCEL) == IDOK)
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
		AngoMsgBox::MessageBox(strMsg,L"按键", MB_POST_CENTER);
	}


	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CAngoDlg::OnOK()
{
	CString strMsg = L"回车键";
	AngoMsgBox::MessageBox(strMsg);

}

void CAngoDlg::OnCancel()
{
	CString strMsg;
	strMsg = "是否退出程序？";
	if( AngoMsgBox::MessageBox(strMsg,L"退出",MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}
	CDialog::OnCancel();
}

//-------------------------------------------------------------------------------------------------------------------------
LRESULT CAngoDlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
{
	if (lparam == WM_LBUTTONDOWN)
	{
		//恢复窗口或者最小化
		if (m_trayIcon.m_bMin == TRUE)
		{
			AfxGetMainWnd()->ShowWindow(SW_SHOW);
			AfxGetMainWnd()->ShowWindow(SW_RESTORE);
			//这里貌似只有写这样两句才能保证恢复窗口后，该窗口处于活动状态（在最前面）
			m_trayIcon.m_bMin = FALSE;
		}
		else
		{
			AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
			m_trayIcon.m_bMin = TRUE;
		}
	}
	else if (lparam == WM_RBUTTONDOWN)
	{
		//弹出右键菜单
		CMenu popMenu;
		popMenu.LoadMenu(IDR_MENU1);				//IDR_MENU_POPUP是在ResourceView中创建并编辑的一个菜单
		CMenu* pmenu = popMenu.GetSubMenu(0);		//弹出的菜单实际上是IDR_MENU_POPUP菜单的某项的子菜单，这里是第一项
		CPoint pos;
		GetCursorPos(&pos);							//弹出菜单的位置，这里就是鼠标的当前位置
		//显示该菜单，第一个参数的两个值分别表示在鼠标的右边显示、响应鼠标右击
		pmenu->TrackPopupMenu( TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
		
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------------
void CAngoDlg::OnUpLayer()
{
	//设置在最上层，取消
	if (m_bUplayer)
	{
		CRect rtClient;
		GetWindowRect(rtClient);
		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), SWP_SHOWWINDOW);
		m_bUplayer = FALSE;
	}
	else
	{
		CRect rtClient;
		GetWindowRect(rtClient);
		::SetWindowPos(m_hWnd, HWND_TOPMOST, rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), SWP_SHOWWINDOW);
		m_bUplayer = TRUE;
	}
}

void CAngoDlg::OnShowDlg()
{
	//恢复窗口或者最小化
	if (m_trayIcon.m_bMin == TRUE)
	{
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->ShowWindow(SW_RESTORE);
		//这里貌似只有写这样两句才能保证恢复窗口后，该窗口处于活动状态（在最前面）
		m_trayIcon.m_bMin = FALSE;
	}
	else
	{
		AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
		m_trayIcon.m_bMin = TRUE;
	}
}

void CAngoDlg::OnAboutDlg()
{
	CString strMsg;
	strMsg = "说明";
	AngoMsgBox::MessageBox(strMsg);
}

void CAngoDlg::OnExitDlg()
{
	OnCancel();
}


void CAngoDlg::OnConfig()
{
	m_pCfgMng = new CConfigMng;
	m_pCfgMng->DoModal();
	delete m_pCfgMng;
	m_pCfgMng = NULL;
}
//-------------------------------------------------------------------------------------------------------------------------



//设置自启动
//输入 wszPath[]绝对路径， wszName[]新建项名
BOOL CAngoDlg::RegAutoStart()
{
	// 32位程序在64位系统运行,操作注册表被重定向的问题.


	HMODULE hModule = GetModuleHandle(NULL);

	//得到自身完整的路径
	//GetModuleFileName(hModule, wszPath, sizeof(wszPath));



	
#if _WIN64
	//AngoMsgBox::MessageBox(L"WIN_64", L"", MB_POST_CENTER);
	HKEY hRoot = HKEY_LOCAL_MACHINE;
	HKEY hKey;
	LONG lRet;
	DWORD dwDisposition;

	char szPath[MAX_PATH] = {0};
	GetModuleFileNameA(hModule, szPath, sizeof(szPath));

	const char *subkey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce";	//RunOnce

	lRet = RegCreateKeyExA(hRoot,
		subkey,
		0,   //保留参数，必须为0
		NULL,//键的种类，同通常为NULL
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisposition  //返回参数
		);
	

	char data_Name[MAX_PATH] = "Ango";

	lRet = ::RegSetValueExA(
		hKey,
		data_Name,
		0,						//保留
		REG_SZ,					//字符串
		(CONST BYTE *)szPath,	//具体内容
		(DWORD)strlen(szPath)
		);

	if (lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		OutputDebugStringA("打开注册表失败");
		return FALSE;
	}

	RegCloseKey(hKey);

#endif



	return TRUE;
}
//-------------------------------------------------------------------------------------------------------------------------


