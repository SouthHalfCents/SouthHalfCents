
// AngoTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AngoTime.h"
#include "AngoTimeDlg.h"
#include "afxdialogex.h"
#include "MsgBoxEx.h"

//playsound
#include <mmsystem.h>
#pragma comment( lib, "Winmm.lib" )

#define PI 3.1415926535897932384626433832795028841971693993751058209

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



HANDLE			g_hThread_Alarm		=	NULL;
HANDLE			g_hThread_SayTime	=	NULL;
HANDLE			g_hThread_CusJob	=	NULL;

HANDLE			g_hSemaph_Alarm		=	NULL;	//闹钟
HANDLE			g_hSemaph_SayTime	=	NULL;	//报时
HANDLE			g_hSemaph_CusJob	=	NULL;	//定时任务

std::mutex		g_MutexSound;					
BOOL			g_bWork				=	FALSE;
DWORD			g_dwTasktype		=	0;

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

	m_cLastHour		=	0;
	m_cLastMin		=	0;
	m_cLastSecL		=	0;
	m_cLastSecS		=	0;

	m_nSayTime		=	SAYTIME_CLOSE;
	m_bClockState	=	FALSE;
	m_bAutoRun		=	FALSE;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_popMenu.LoadMenu(IDR_MENU_RBTN);	
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
	ON_COMMAND(ID_CFG_AUTORUN, &CAngoTimeDlg::OnCfgAutorun)
	ON_COMMAND(ID_CLOCK_ON, &CAngoTimeDlg::OnClockOn)
	ON_COMMAND(ID_CLOCK_OFF, &CAngoTimeDlg::OnClockOff)
	ON_COMMAND(ID_CLOCK_CONFIG, &CAngoTimeDlg::OnClockConfig)
	ON_COMMAND(ID_MENU_TASK, &CAngoTimeDlg::OnMenuTask)
END_MESSAGE_MAP()


// CAngoTimeDlg 消息处理程序
void CAngoTimeDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值


	CleanThread();


	CDialogEx::OnClose();
	CDialogEx::OnCancel();
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
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, L"//AngoTime.exe");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 如果程序已经存在并且正在运行 
		// 如果已有互斥量存在则释放句柄并复位互斥量，退出程序
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		//AngoMessageBox(_T("程序已经在运行"));
		CDialog::OnCancel();
	}

	InitClock();


	//调整位置
	CRect cr;
	GetClientRect(&cr);//获取对话框客户区域大小
	ClientToScreen(&cr);//转换为荧幕坐标
	int x = GetSystemMetrics(SM_CXSCREEN);//获取荧幕坐标的宽度，单位为像素
	int y = GetSystemMetrics(SM_CYSCREEN);//获取荧幕坐标的高度，单位为像素
	//MoveWindow((x-cr.Width() *2)/2 ,cr.top,cr.Width() *2,cr.Height() *2);//左上角

	MoveWindow(x - cr.Width(), cr.Height(), cr.Width(), cr.Height());

	OnViewShow();
	OnViewUp();
	InitSettings();
	InitThread();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	OnClose();
}

//-------------------------------------------------------------------------------------------------------------------------
LRESULT CAngoTimeDlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
{
	if (lparam == WM_LBUTTONDOWN)
	{
		//恢复窗口或者最小化
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->ShowWindow(SW_RESTORE);
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
void CAngoTimeDlg::InitClock()
{
	//设置控制时针走动的触发器为每秒一次
	m_uClock_Timer = this->SetTimer(1, 1000, NULL);

	//画圆形对话框
	CRgn  rgn;
	CRect  rc;
	GetClientRect(&rc);
	rgn.CreateEllipticRgn(rc.left, rc.top, rc.right-1, rc.bottom-1);
	SetWindowRgn(rgn, TRUE);
	rgn.DeleteObject();
	
	//用时钟背景图片作圆形对话框背景
	CBitmap   bm;
	bm.LoadBitmap(IDB_BMP_CLOCK);		//   可以指定bitmap图片的路径   
	m_cBrush.CreatePatternBrush(&bm);
}

HBRUSH CAngoTimeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何属性
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return m_cBrush;
}



void  CAngoTimeDlg::ClockTime()
{
	//获得当前系统时间。
	CTime time = CTime::GetCurrentTime();
	CPen *pPenOld = NULL;
	CPen PenNew;
	CBrush *pBrushOld = NULL;
	CBrush BrushNew;
	CClientDC dc(this);

	int   S = time.GetSecond();
	float M = float(time.GetMinute() + S / 60.0);
	float H = float(time.GetHour() + M / 60.0);
	if (H > 12)
		H = H - 12;
	H = H * 5;

	m_Point_Start.x = 65;
	m_Point_Start.y = 64;
	COLORREF cRgb	= 0;

	
	//方法为画每根针前用背景色擦去上一次画的针（由于背景色渐变，所以加入了计算）
	//从图片获取背景色rgb
	//COLORREF cRgb = dc.GetPixel(m_Point_End);
	//BYTE byRed   = GetRValue(cRgb);
	//BYTE byGreen = GetGValue(cRgb);
	//BYTE byBlue	 = GetBValue(cRgb);

	//////////////////////////////////////////////	
	if ( !m_bFirstClock )
	{
		//用背景色擦去上一次画的针
		cRgb = m_cLastHour;
		m_Point_End = m_point_lastHour;

		PenNew.CreatePen(PS_SOLID, 4, cRgb);
		BrushNew.CreateSolidBrush(cRgb);
		pBrushOld = dc.SelectObject(&BrushNew);
		pPenOld = dc.SelectObject(&PenNew);
		dc.MoveTo(m_Point_Start);
		dc.LineTo(m_Point_End);
	}

	//画时针
	PenNew.DeleteObject();
	PenNew.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	pPenOld = dc.SelectObject(&PenNew);
	BrushNew.DeleteObject();
	BrushNew.CreateSolidBrush(RGB(0, 0, 0));
	pBrushOld = dc.SelectObject(&BrushNew);
	m_Point_End.x = 65 + LONG(22 * sin(H*PI / 30));
	m_Point_End.y = 64 - LONG(22 * cos(H*PI / 30));

	//保存原有的rgb
	m_cLastHour = dc.GetPixel(m_Point_End);
	m_point_lastHour = m_Point_End;

	dc.MoveTo(m_Point_Start);
	dc.LineTo(m_Point_End);
	


	///////////////////////////////////////////////
	if ( !m_bFirstClock )
	{
		//擦除上次的指针
		cRgb = m_cLastMin;
		m_Point_End = m_point_lastMin;

		BrushNew.DeleteObject();
		BrushNew.CreateSolidBrush(cRgb);
		pBrushOld = dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_SOLID, 3, cRgb);
		pPenOld = dc.SelectObject(&PenNew);
		dc.MoveTo(m_Point_Start);
		dc.LineTo(m_Point_End);
	}

	//画分针
	BrushNew.DeleteObject();
	BrushNew.CreateSolidBrush(RGB(0, 0, 0));
	pBrushOld = dc.SelectObject(&BrushNew);
	PenNew.DeleteObject();
	PenNew.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	pPenOld = dc.SelectObject(&PenNew);
	m_Point_End.x = 65 + LONG(30 * sin(M*PI / 30));
	m_Point_End.y = 64 - LONG(30 * cos(M*PI / 30));

	m_cLastMin = dc.GetPixel(m_Point_End);
	m_point_lastMin = m_Point_End;

	dc.MoveTo(m_Point_Start);
	dc.LineTo(m_Point_End);
	

	////////////////////////////////////////////	画秒针的短轴	
	S   = (S + 30) % 60;
	
	if ( !m_bFirstClock )
	{
		//擦除上次的指针
		cRgb = m_cLastSecS;
		m_Point_End = m_point_lastSecS;

		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT, 2, cRgb);
		pPenOld = dc.SelectObject(&PenNew);
		dc.MoveTo(m_Point_Start);
		dc.LineTo(m_Point_End);
	}


	//画秒针短轴
	PenNew.DeleteObject();
	PenNew.CreatePen(PS_DASHDOTDOT, 2, RGB(255, 0, 0));
	pPenOld = dc.SelectObject(&PenNew);
	m_Point_End.x = 65 + LONG(6 * sin(S*PI / 30));
	m_Point_End.y = 64 - LONG(6 * cos(S*PI / 30));

	m_cLastSecS = dc.GetPixel(m_Point_End);
	m_point_lastSecS = m_Point_End;

	dc.MoveTo(m_Point_Start);
	dc.LineTo(m_Point_End);
	


	///////////////////////画秒针的长轴
	S   = (S + 30) % 60;
	
	if ( !m_bFirstClock )
	{
		//擦除上次的指针
		cRgb = m_cLastSecL;
		m_Point_End = m_point_lastSecL;

		//		BrushNew.DeleteObject();
		//		BrushNew.CreateSolidBrush(RGB(C,C,C));
		//		BrushOld=dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT, 2, cRgb);
		pPenOld = dc.SelectObject(&PenNew);
		dc.MoveTo(m_Point_Start);
		dc.LineTo(m_Point_End);
	}


	//画秒针长轴
	//		BrushNew.DeleteObject();
	//		BrushNew.CreateSolidBrush(RGB(255,0,0));
	//		BrushOld=dc.SelectObject(&BrushNew);
	PenNew.DeleteObject();
	PenNew.CreatePen(PS_DASHDOTDOT, 2, RGB(255, 0, 0));
	pPenOld = dc.SelectObject(&PenNew);
	m_Point_End.x = 65 + LONG(30 * sin(S*PI / 30));
	m_Point_End.y = 64 - LONG(30 * cos(S*PI / 30));

	m_cLastSecL = dc.GetPixel(m_Point_End);
	m_point_lastSecL = m_Point_End;

	dc.MoveTo(m_Point_Start);
	dc.LineTo(m_Point_End);



	//////////////////////////////////////////////////////////////////
	m_bFirstClock = FALSE;
	
	dc.SetPixel(m_Point_Start, RGB(0, 0, 0));
	dc.SetPixel(m_Point_Start.x + 1, m_Point_Start.y,	  RGB(0, 0, 0));
	dc.SetPixel(m_Point_Start.x,	 m_Point_Start.y + 1, RGB(0, 0, 0));
	dc.SetPixel(m_Point_Start.x + 1, m_Point_Start.y + 1, RGB(0, 0, 0));
	dc.SetPixel(m_Point_Start.x - 1, m_Point_Start.y,	  RGB(0, 0, 0));
	dc.SetPixel(m_Point_Start.x,	 m_Point_Start.y - 1, RGB(0, 0, 0));
	dc.SetPixel(m_Point_Start.x - 1, m_Point_Start.y - 1, RGB(0, 0, 0));

	//读取系统配置
	//AfxGetApp()->WriteProfileInt("Sound","hoursound",temp);
	if (m_nSayTime==SAYTIME_ALL &&S == 0 && M == 0)//判断是否整点报时
		OnSaytimeNow();
	//判断是否半点报时
	if (m_nSayTime==SAYTIME_HALF &&S == 0 && (M == 0 || M == 30))
		OnSaytimeNow();

}


//-------------------------------------------------------------------------------------------------------------------------



void CAngoTimeDlg::OnTimer(UINT_PTR nIDEvent)//控制时钟走动
{
	//判断传递过来的时钟触发器是否是自己定义的时钟触发器

	if (nIDEvent = m_uClock_Timer)
	{
		ClockTime();
	}
	else
	{

// 		for(int i=0;i<ringnum;i++)//判断是否有闹钟应被执行
// 		{
// 			if(time.GetHour()==mytimearray[i].hour&&time.GetMinute()==mytimearray[i].minute&&time.GetSecond()==mytimearray[i].second)
// 			{
// 				pThread2->ResumeThread();
// 				break;
// 			}
// 		}



// 		for(int i=0;i<tasknum;i++)//判断是否有定时任务应被执行
// 		{
// 			if(time.GetYear()==mytaskarray[i].year&&time.GetMonth()==mytaskarray[i].month&&time.GetDay()==mytaskarray[i].day)
// 			{
// 				if(time.GetHour()==mytaskarray[i].hour&&time.GetMinute()==mytaskarray[i].minute&&time.GetSecond()==mytaskarray[i].second)
// 				{
// 					tasktyple=mytaskarray[i].typle;
// 					pThread3->ResumeThread();
// 					break;
// 				}
// 			}
// 		}


// 		if(hoursound&&S==0&&M==0)//判断是否整点报时
// 			SoundTime();
// 		//判断是否半点报时
// 		if(halfhoursound&&S==0&&(M==0||M==30))
// 			SoundTime();
		//this->UpdateData(false);
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
	AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);

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
	//这个API会提示是否以管理员身份启动
	ShellExecute(NULL, L"open", L"Ango.exe", NULL, NULL, SW_SHOWNORMAL);

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
	OnCancel();
}

//-------------------------------------------------------------------------------------------------------------------------
void InitThread()
{
	if ( g_hThread_Alarm || g_hThread_SayTime || g_hThread_CusJob )
		return;

	g_bWork = TRUE;

	//创建信号量
	g_hSemaph_Alarm		= CreateSemaphore(NULL, 0, 2, _T("AngoTime_Alarm"));
	g_hSemaph_SayTime	= CreateSemaphore(NULL, 0, 2, _T("AngoTime_SayTime"));
	g_hSemaph_CusJob	= CreateSemaphore(NULL, 0, 2, _T("AngoTime_CusJob"));
	/*
	第一个参数：安全属性，如果为NULL则是默认安全属性 
	第二个参数：信号量的初始值，要>=0且<=第三个参数
	第三个参数：信号量的最大值
	第四个参数：信号量的名称
	*/
	

	//创建线程
	g_hThread_Alarm		= (HANDLE)_beginthreadex(NULL, 0, Thread_Alarm, NULL, 0, NULL);
	g_hThread_SayTime	= (HANDLE)_beginthreadex(NULL, 0, Thread_SayTime, NULL, 0, NULL);
	g_hThread_CusJob	= (HANDLE)_beginthreadex(NULL, 0, Thread_CusJob, NULL, 0, NULL);

	
	
}

void CleanThread()
{
	g_bWork = FALSE;

	ReleaseSemaphore(g_hSemaph_Alarm, 1, NULL);		//释放1个信号量
	ReleaseSemaphore(g_hSemaph_SayTime, 1, NULL);	//释放1个信号量
	ReleaseSemaphore(g_hSemaph_CusJob, 1, NULL);	//释放1个信号量


	WaitForSingleObject(g_hThread_Alarm, INFINITE);
	WaitForSingleObject(g_hThread_SayTime, INFINITE);
	WaitForSingleObject(g_hThread_CusJob, INFINITE);

	CloseHandle(g_hThread_Alarm);
	CloseHandle(g_hThread_SayTime);
	CloseHandle(g_hThread_CusJob);
	CloseHandle(g_hSemaph_Alarm);
	CloseHandle(g_hSemaph_SayTime);
	CloseHandle(g_hSemaph_CusJob);
}

unsigned int __stdcall Thread_Alarm(LPVOID pParam)
{
	CString strPath;
	while (g_bWork)
	{
		WaitForSingleObject(g_hSemaph_Alarm,INFINITE);
		if (!g_bWork)
		{
			break;
		}

		g_MutexSound.lock();
		strPath = "c:\\win95\\media\\The Microsoft Sound.wav";
		PlaySound(strPath, NULL, SND_FILENAME | SND_ASYNC);
// 		PlaySound(MAKEINTRESOURCE(IDR_WAVESOUND), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
// 		Sleep(7000);
// 		PlaySound(MAKEINTRESOURCE(IDR_WAVESOUND), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
// 		Sleep(8000);
		
		g_MutexSound.unlock();
	}
	
	TRACE("exit ThProcAlarm\n");
	return 0;
}
unsigned int __stdcall Thread_SayTime(LPVOID pParam)
{
	while (g_bWork)
	{
		WaitForSingleObject(g_hSemaph_SayTime, INFINITE);
		if (!g_bWork)
		{
			break;
		}

		g_MutexSound.lock();

		CTime m_NowTime;
		m_NowTime = CTime::GetCurrentTime();
		int n_Hour = m_NowTime.GetHour();
		//以下为报时代码,原理为通过对当前时间的判断,调用不同的声音文件(资源文件)进行组合播放,形成报时效果
		PlaySound(MAKEINTRESOURCE(IDR_WAVE_TIMENOW), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		Sleep(1550);
		if (n_Hour < 8)
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_MORNING), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		else if (n_Hour < 13)
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_AM), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		else if (n_Hour < 19)
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_PM), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		else
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_EM), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		if (n_Hour > 12)
		{
			n_Hour = n_Hour - 12;
		}
		int n_Minute = m_NowTime.GetMinute();
		int n_Second = m_NowTime.GetSecond();



		Sleep(580);
		switch (n_Hour)
		{
		case 0:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T00), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 1:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T01), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 2:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T2), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 3:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T03), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 4:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T04), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 5:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T05), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 6:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T06), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 7:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T07), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 8:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T08), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 9:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T09), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 10:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T10), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 11:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T11), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 12:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T12), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		}
		if (n_Hour < 10)
			Sleep(450);
		else
			Sleep(550);
		PlaySound(MAKEINTRESOURCE(IDR_WAVE_POINT), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		Sleep(500);

		int n_Front_Min;
		int n_Behind_Min;
		int n_FTemp = n_Minute;

		n_Front_Min = n_Minute / 10;
		n_Behind_Min = n_Minute % 10;

		if (n_Front_Min <= 0)
		{
			if (n_FTemp != 0)
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T00), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			Sleep(400);
			switch (n_FTemp)
			{
			case 0:
				break;
			case 1:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T01), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 2:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T02), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 3:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T03), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 4:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T04), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 5:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T05), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 6:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T06), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 7:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T07), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 8:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T08), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 9:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T09), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			default:
				break;
			}
			Sleep(500);
			if (n_FTemp != 0)
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_MIN), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);

		}
		else
		{
			switch (n_Front_Min)
			{
			case 1:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T10), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 2:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T20), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 3:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T30), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 4:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T40), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 5:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T50), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			}
			if (n_Front_Min == 1)
				Sleep(470);
			else if (n_Front_Min == 4)
				Sleep(565);
			else if (n_Behind_Min > 0)
				Sleep(520);
			switch (n_Behind_Min)
			{
			case 1:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T01), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 2:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T02), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 3:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T03), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 4:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T04), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 5:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T05), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 6:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T06), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 7:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T07), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 8:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T08), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 9:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T09), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			}

			Sleep(560);
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_MIN), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		}
		
		g_MutexSound.unlock();
		

	}

	TRACE("exit ThProcSayTime\n");
	return 0;
}
unsigned int __stdcall Thread_CusJob(LPVOID pParam)
{
	while (g_bWork)
	{
		WaitForSingleObject(g_hSemaph_CusJob, INFINITE);
		if (!g_bWork)
		{
			break;
		}
		
	}

	TRACE("exit ThProcWork\n");
	return 0;
}




void CAngoTimeDlg::OnSaytimeNow()
{
	ReleaseSemaphore(g_hSemaph_SayTime, 1, NULL);	//释放1个信号量
}



void CAngoTimeDlg::OnSaytimeAll()
{
	m_nSayTime = SAYTIME_ALL;

	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(6);
		if (pMenu)
		{
			pMenu->CheckMenuItem(ID_SAYTIME_ALL, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(ID_SAYTIME_HALF, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_SAYTIME_CLOSE, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION, SAYTIME_ENTRY, m_nSayTime);
}


void CAngoTimeDlg::OnSaytimeHalf()
{
	m_nSayTime = SAYTIME_HALF;

	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(6);
		if (pMenu)
		{
			pMenu->CheckMenuItem(ID_SAYTIME_ALL, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_SAYTIME_HALF, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(ID_SAYTIME_CLOSE, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION, SAYTIME_ENTRY, m_nSayTime);
}


void CAngoTimeDlg::OnSaytimeClose()
{
	m_nSayTime = SAYTIME_CLOSE;

	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(6);
		if (pMenu)
		{
			pMenu->CheckMenuItem(ID_SAYTIME_ALL, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_SAYTIME_HALF, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_SAYTIME_CLOSE, MF_BYCOMMAND | MF_CHECKED);
		}
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION, SAYTIME_ENTRY, m_nSayTime);
}


void CAngoTimeDlg::OnCfgAutorun()
{
	//开机启动...

	m_bAutoRun = !m_bAutoRun;


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
		lRet = ::RegDeleteValueA(hKey,data_Name);   
	}


	if (lRet != ERROR_SUCCESS)
	{	
		OutputDebugStringA("打开注册表失败");
	}

	RegCloseKey(hKey);

#endif



	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(8);
		if (pMenu)
		{
			if (m_bAutoRun)
				pMenu->CheckMenuItem(ID_CFG_AUTORUN, MF_BYCOMMAND | MF_CHECKED);
			else
				pMenu->CheckMenuItem(ID_CFG_AUTORUN, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION, AUTORUN_ENTRY, m_bAutoRun);
}


void CAngoTimeDlg::OnClockOn()
{
	m_bClockState = TRUE;

	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(5);
		if (pMenu)
		{
			pMenu->CheckMenuItem(ID_CLOCK_ON, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(ID_CLOCK_OFF, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION, CLOCK_ENTRY, m_bClockState);
}


void CAngoTimeDlg::OnClockOff()
{
	m_bClockState = FALSE;

	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	if (pMenu)
	{
		pMenu = pMenu->GetSubMenu(5);
		if (pMenu)
		{
			pMenu->CheckMenuItem(ID_CLOCK_ON, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_CLOCK_OFF, MF_BYCOMMAND | MF_CHECKED);
		}
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION, CLOCK_ENTRY, m_bClockState);
}


void CAngoTimeDlg::OnClockConfig()
{
	// TODO:  在此添加命令处理程序代码
}


void CAngoTimeDlg::OnMenuTask()
{
	// TODO:  在此添加命令处理程序代码
}
//---------------------------------------------------------------------------------------------------------------------
void CAngoTimeDlg::InitSettings()
{
	//闹钟设置
	m_bClockState = AfxGetApp()->GetProfileInt(ANGO_SECTION, CLOCK_ENTRY, 0);
	if (m_bClockState)
	{
		OnClockOn();
	}
	else
	{
		OnClockOff();
	}


	//报时设置
	m_nSayTime = AfxGetApp()->GetProfileInt(ANGO_SECTION, SAYTIME_ENTRY, 0);
	if (SAYTIME_CLOSE == m_nSayTime)
	{
		OnSaytimeClose();
	}
	else if (SAYTIME_ALL == m_nSayTime)
	{
		OnSaytimeAll();
	}
	else if (SAYTIME_HALF == m_nSayTime)
	{
		OnSaytimeHalf();
	}

	//开机启动设置
	m_bAutoRun = AfxGetApp()->GetProfileInt(ANGO_SECTION, AUTORUN_ENTRY, 0);
	m_bAutoRun = !m_bAutoRun;
	OnCfgAutorun();

}
//---------------------------------------------------------------------------------------------------------------------
