
// AngoTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AngoTime.h"
#include "AngoTimeDlg.h"
#include "afxdialogex.h"
#include "MsgBoxEx.h"

#include <afxinet.h>
#include <string>

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAngoTimeDlg 对话框


CAngoTimeDlg::CAngoTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngoTimeDlg::IDD, pParent)
	, m_uClock_Timer(0)
	, m_Point_Start(0)
	, m_Point_End(0)
{
	m_point_lastHour	=	0;
	m_point_lastMin		=	0;
	m_point_lastSecL	=	0;
	m_point_lastSecS	=	0;
	m_bFirstClock		=	TRUE;


	m_nSayTime		=	SAYTIME_CLOSE;
	m_bClockOn		=	FALSE;
	m_bAutoRun		=	FALSE;
	m_hOnlyMutex	=	NULL;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_popMenu.LoadMenu(IDR_MENU_RBTN);	
}

CAngoTimeDlg::~CAngoTimeDlg()
{

}

void CAngoTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAngoTimeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()			//定时器
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()

	ON_MESSAGE(MAIN_WM_NOTIFYICON, &CAngoTimeDlg::OnNotifyIcon)
	ON_COMMAND(ID_MENU_EXIT, &CAngoTimeDlg::OnMenuExit)
	ON_COMMAND(ID_VIEW_UP, &CAngoTimeDlg::OnViewUp)
	ON_COMMAND(ID_VIEW_DOWN, &CAngoTimeDlg::OnViewDown)
	ON_COMMAND(ID_VIEW_SHOW, &CAngoTimeDlg::OnViewShow)
	ON_COMMAND(ID_VIEW_HIDE, &CAngoTimeDlg::OnViewHide)
	ON_COMMAND(ID_MENU_ANGO, &CAngoTimeDlg::OnMenuAngo)
	ON_WM_CLOSE()
	ON_COMMAND(ID_SAYTIME_NOW, &CAngoTimeDlg::OnSaytimeNow)
	ON_COMMAND(ID_SAYTIME_ALL, &CAngoTimeDlg::OnSaytimeAll)
	ON_COMMAND(ID_SAYTIME_HALF, &CAngoTimeDlg::OnSaytimeHalf)
	ON_COMMAND(ID_SAYTIME_CLOSE, &CAngoTimeDlg::OnSaytimeClose)
	ON_COMMAND(ID_CFG_AUTORUN, &CAngoTimeDlg::OnCfgAutoStart)
	ON_COMMAND(ID_CLOCK_ON, &CAngoTimeDlg::OnClockOn)
	ON_COMMAND(ID_CLOCK_OFF, &CAngoTimeDlg::OnClockOff)
	ON_COMMAND(ID_CLOCK_CONFIG, &CAngoTimeDlg::OnClockConfig)
	ON_COMMAND(ID_MENU_TASK, &CAngoTimeDlg::OnMenuTask)
	ON_COMMAND(ID_CFG_OTHER, &CAngoTimeDlg::OnCfgOther)
END_MESSAGE_MAP()


// CAngoTimeDlg 消息处理程序
void CAngoTimeDlg::OnClose()
{
	StopWork();

	if (m_hOnlyMutex)
	{
		CloseHandle(m_hOnlyMutex);
		m_hOnlyMutex = NULL;
	}
	CDialogEx::OnClose();

}

BOOL CAngoTimeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//隐藏任务栏
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// 唯一实例
	m_hOnlyMutex = CreateMutex(NULL, FALSE, L"//AngoTime.exe");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 如果程序已经存在并且正在运行 如果已有互斥量存在则释放句柄并复位互斥量，退出程序
		CloseHandle(m_hOnlyMutex);
		m_hOnlyMutex = NULL;
		CDialog::OnCancel();
		return TRUE;
	}

	InitClock();
	InitPosition();

	InitRgbMap();
	StartWork((void*)this);
	OnViewShow();
	//OnViewUp();
	InitSettings();
	

	return TRUE;  
}





void CAngoTimeDlg::OnPaint()
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
		OnTimer(m_uClock_Timer);
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CAngoTimeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//重载windows帮助，屏蔽F1帮助
void CAngoTimeDlg::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	return;
	//CDialog::WinHelp(dwData, nCmd);
}

int CAngoTimeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//通知区域图标，托盘图标
	m_trayIcon.m_pCwnd = this;
	m_trayIcon.m_dwIconId = IDR_MAINFRAME;
	m_trayIcon.m_strTips = "╮(￣▽￣)╭ ";
	m_trayIcon.InitTrayIcon();
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------
//拖动无标题对话框窗口
void CAngoTimeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

//拖动无标题对话框窗口
void CAngoTimeDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu* pmenu = m_popMenu.GetSubMenu(0);		//弹出的菜单实际上是IDR_MENU_POPUP菜单的某项的子菜单，这里是第一项
	CPoint pos;
	GetCursorPos(&pos);							//弹出菜单的位置，这里就是鼠标的当前位置
	//显示该菜单，第一个参数的两个值分别表示在鼠标的右边显示、响应鼠标右击
	pmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
	CDialogEx::OnRButtonDown(nFlags, point);
}

void CAngoTimeDlg::OnOK()
{
}

void CAngoTimeDlg::OnCancel()
{
	//OnClose();
	//CDialogEx::OnCancel();
}

//-------------------------------------------------------------------------------------------------------------------------
LRESULT CAngoTimeDlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
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
		pmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------------
void CAngoTimeDlg::InitPosition()
{
	//调整位置
	CRect cr;
	GetClientRect(&cr);//获取对话框客户区域大小
	ClientToScreen(&cr);//转换为荧幕坐标
	int x = GetSystemMetrics(SM_CXSCREEN);//获取荧幕坐标的宽度，单位为像素
	int y = GetSystemMetrics(SM_CYSCREEN);//获取荧幕坐标的高度，单位为像素
	//MoveWindow((x-cr.Width() *2)/2 ,cr.top,cr.Width() *2,cr.Height() *2);//左上角
	MoveWindow(x - cr.Width(), cr.Height(), cr.Width(), cr.Height());
}
void CAngoTimeDlg::InitClock()
{
	//设置控制时针走动的触发器为每秒一次
	m_uClock_Timer = this->SetTimer(1, 1000, NULL);

	//画圆形对话框
	CRgn  rgn;
	CRect  rc;
	GetClientRect(&rc);
	rgn.CreateEllipticRgn(rc.left+2, rc.top+2, rc.right-3, rc.bottom-3);
	SetWindowRgn(rgn, TRUE);
	rgn.DeleteObject();
	
	//用时钟背景图片作圆形对话框背景
	CBitmap   bm;
	bm.LoadBitmap(IDB_BMP_CLOCK);		//   可以指定bitmap图片的路径   
	BOOL bRet = m_cBrush.CreatePatternBrush(&bm);
}

HBRUSH CAngoTimeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何属性
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return m_cBrush;
}

void CAngoTimeDlg::InitRgbMap()
{
	CBitmap bitmap;
	BOOL bLoad = bitmap.LoadBitmap(IDB_BMP_CLOCK);
	CSize csize = bitmap.GetBitmapDimension();

	COLORREF C = 0;
	int R = GetRValue(C);
	int G = GetGValue(C);
	int B = GetBValue(C);


	bitmap.GetBitmap(&m_bmpClock);
	int nbyte = m_bmpClock.bmBitsPixel / 8;

	BITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = m_bmpClock.bmWidth;
	bi.bmiHeader.biHeight = -m_bmpClock.bmHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = m_bmpClock.bmBitsPixel;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = m_bmpClock.bmWidth * m_bmpClock.bmHeight * nbyte;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	// 获取位图数据  
	HDC hdc = ::GetDC(NULL);
	BYTE* pBits = (BYTE*)new BYTE[m_bmpClock.bmWidth * m_bmpClock.bmHeight * nbyte];
	::ZeroMemory(pBits, m_bmpClock.bmWidth * m_bmpClock.bmHeight * nbyte);
	if (!::GetDIBits(hdc, bitmap, 0, m_bmpClock.bmHeight, pBits, &bi, DIB_RGB_COLORS))
	{
		delete pBits;
		pBits = NULL;
		AngoMessageBox(L"初始化时钟位图失败");
		OnMenuExit();
	}

	m_vBmpColor.resize(m_bmpClock.bmWidth);
	for (int i = 0; i < m_bmpClock.bmWidth; ++i)
	{
		m_vBmpColor[i].resize(m_bmpClock.bmHeight);
		for (int j = 0; j < m_bmpClock.bmHeight; ++j)
		{
			R = pBits[i * nbyte + j * m_bmpClock.bmWidthBytes + 2];
			G = pBits[i * nbyte + j * m_bmpClock.bmWidthBytes + 1];
			B = pBits[i * nbyte + j * m_bmpClock.bmWidthBytes + 0];
			
			C = RGB(R,G,B);
			m_vBmpColor[i][j] = C;
		}
	}


	delete pBits;
	pBits = NULL;

}




//-------------------------------------------------------------------------------------------------------------------------
void CAngoTimeDlg::OnTimer(UINT_PTR nIDEvent)//控制时钟走动
{
	//判断传递过来的时钟触发器是否是自己定义的时钟触发器

	if (nIDEvent = m_uClock_Timer)
	{
		ReleaseSemaphore(g_hSemaph_Clock, 1, NULL);
	}
	else
	{

	}
	CDialog::OnTimer(nIDEvent);
}


//-------------------------------------------------------------------------------------------------------------------------

void CAngoTimeDlg::OnViewUp()
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
			pMenu->CheckMenuItem(ID_VIEW_UP, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(ID_VIEW_DOWN, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}
}


void CAngoTimeDlg::OnViewDown()
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
			pMenu->CheckMenuItem(ID_VIEW_UP, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_VIEW_DOWN, MF_BYCOMMAND | MF_CHECKED);
		}
	}
}


void CAngoTimeDlg::OnViewShow()
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
			pMenu->CheckMenuItem(ID_VIEW_SHOW, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(ID_VIEW_HIDE, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}
}


void CAngoTimeDlg::OnViewHide()
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
			pMenu->CheckMenuItem(ID_VIEW_SHOW, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_VIEW_HIDE, MF_BYCOMMAND | MF_CHECKED);
		}
	}
}


void CAngoTimeDlg::OnMenuAngo()
{
	CString strPath = CUtils::GetAppDir();
	strPath += _T("\\Ango.exe");
	//这个API会提示是否以管理员身份启动
	ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL);

// 	PROCESS_INFORMATION pi;
// 	STARTUPINFO si;
// 	memset(&si, 0, sizeof(si));
// 	si.cb = sizeof(si);
// 	si.wShowWindow = SW_SHOW;
// 	si.dwFlags = STARTF_USESHOWWINDOW;
// 	BOOL fRet = CreateProcess(L"Ango.exe", NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &si, &pi);
}

void CAngoTimeDlg::OnMenuExit()
{
	OnClose();
	CDialogEx::OnCancel();
}






void CAngoTimeDlg::OnSaytimeNow()
{
	ReleaseSemaphore(g_hSemaph_SayTime, 1, NULL);	//释放1个信号量
}

BOOL CAngoTimeDlg::SetSayTimeState()
{
	UINT uCheck_Close = MF_BYCOMMAND | MF_UNCHECKED;
	UINT uCheck_All   = MF_BYCOMMAND | MF_UNCHECKED;
	UINT uCheck_Half  = MF_BYCOMMAND | MF_UNCHECKED;


	CMenu* pMenu = m_popMenu.GetSubMenu(MENU_NO_FIRST);
	if (!pMenu)
		return FALSE;
	pMenu = pMenu->GetSubMenu(MENU_SAYTIME);
	if (!pMenu)
		return FALSE;


	switch (m_nSayTime)
	{
	case SAYTIME_CLOSE:
	{
		uCheck_Close |= MF_CHECKED;
	}
	break;
	case SAYTIME_ALL:
	{
		uCheck_All |= MF_CHECKED;
	}
	break;
	case SAYTIME_HALF:
	{
		uCheck_Half |= MF_CHECKED;
	}
	break;
	default:
	{
		uCheck_Close |= MF_CHECKED;
	}
		break;
	}

	pMenu->CheckMenuItem(ID_SAYTIME_ALL, uCheck_All);
	pMenu->CheckMenuItem(ID_SAYTIME_HALF, uCheck_Half);
	pMenu->CheckMenuItem(ID_SAYTIME_CLOSE, uCheck_Close);
	return TRUE;
}

void CAngoTimeDlg::OnSaytimeAll()
{
	m_nSayTime = SAYTIME_ALL;
	if (SetSayTimeState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, SAYTIME_ENTRY, m_nSayTime);
}


void CAngoTimeDlg::OnSaytimeHalf()
{
	m_nSayTime = SAYTIME_HALF;
	if (SetSayTimeState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, SAYTIME_ENTRY, m_nSayTime);
}


void CAngoTimeDlg::OnSaytimeClose()
{
	m_nSayTime = SAYTIME_CLOSE;
	if (SetSayTimeState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, SAYTIME_ENTRY, m_nSayTime);
}

BOOL CAngoTimeDlg::SetAutoStart()
{
	CMenu* pMenu = m_popMenu.GetSubMenu(MENU_NO_FIRST);
	if (!pMenu)
		return FALSE;
	pMenu = pMenu->GetSubMenu(MENU_CONFIG);
	if (!pMenu)
		return FALSE;

	if (m_bAutoRun)
		pMenu->CheckMenuItem(ID_CFG_AUTORUN, MF_BYCOMMAND | MF_CHECKED);
	else
		pMenu->CheckMenuItem(ID_CFG_AUTORUN, MF_BYCOMMAND | MF_UNCHECKED);


#if _WIN64
	HMODULE hModule = GetModuleHandle(NULL);
	HKEY hRoot = HKEY_LOCAL_MACHINE;
	HKEY hKey;
	LONG lRet;
	DWORD dwDisposition;

	char szPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(hModule, szPath, sizeof(szPath));

	const char *subkey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";	//RunOnce

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
	if (m_bAutoRun)
	{
		lRet = ::RegSetValueExA(
			hKey,
			data_Name,
			0,						//保留
			REG_SZ,					//字符串
			(CONST BYTE *)szPath,	//具体内容
			(DWORD)strlen(szPath)
			);
	}
	else
	{
		lRet = ::RegDeleteValueA(hKey, data_Name);
	}


	if (lRet != ERROR_SUCCESS)
	{
		OutputDebugStringA("打开注册表失败\n");
		RegCloseKey(hKey);
		return FALSE;
	}

	RegCloseKey(hKey);

#endif





	return TRUE;
}

void CAngoTimeDlg::OnCfgAutoStart()
{
	m_bAutoRun = !m_bAutoRun;
	if (SetAutoStart() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, AUTORUN_ENTRY, m_bAutoRun);
}


void CAngoTimeDlg::OnClockOn()
{
	m_bClockOn = TRUE;
	if (SetClockState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, CLOCK_ENTRY, m_bClockOn);
}


void CAngoTimeDlg::OnClockOff()
{
	m_bClockOn = FALSE;
	if (SetClockState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, CLOCK_ENTRY, m_bClockOn);
}

BOOL CAngoTimeDlg::SetClockState()
{
	CMenu* pMenu = m_popMenu.GetSubMenu(MENU_NO_FIRST);
	if (!pMenu)
		return FALSE;
	
	pMenu = pMenu->GetSubMenu(MENU_CLOCK);
	if (!pMenu)
		return FALSE;

	if (m_bClockOn)
	{
		pMenu->CheckMenuItem(ID_CLOCK_ON, MF_BYCOMMAND | MF_CHECKED);
		pMenu->CheckMenuItem(ID_CLOCK_OFF, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else
	{
		pMenu->CheckMenuItem(ID_CLOCK_ON, MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_CLOCK_OFF, MF_BYCOMMAND | MF_CHECKED);
	}

	return TRUE;
}

void CAngoTimeDlg::OnClockConfig()
{
	CClock clock_dlg;
	if (IDOK == clock_dlg.DoModal())
	{

	}
}


void CAngoTimeDlg::OnMenuTask()
{
	CTask task_dlg;
	if (IDOK == task_dlg.DoModal())
	{

	}
}

void CAngoTimeDlg::OnCfgOther()
{
	CConfig config_dlg;
	if (IDOK == config_dlg.DoModal())
	{

	}
}

//---------------------------------------------------------------------------------------------------------------------
void CAngoTimeDlg::InitSettings()
{
	//闹钟设置
	m_bClockOn = AfxGetApp()->GetProfileInt(ANGO_SECTION_SET, CLOCK_ENTRY, 0);
	SetClockState();

	//报时设置
	m_nSayTime = AfxGetApp()->GetProfileInt(ANGO_SECTION_SET, SAYTIME_ENTRY, 0);
	SetSayTimeState();

	//开机启动设置
	m_bAutoRun = AfxGetApp()->GetProfileInt(ANGO_SECTION_SET, AUTORUN_ENTRY, 0);
	SetAutoStart();

	//AfxGetApp()->GetProfileString(ANGO_SECTION_SET, AUTORUN_ENTRY, 0);
}
//---------------------------------------------------------------------------------------------------------------------



