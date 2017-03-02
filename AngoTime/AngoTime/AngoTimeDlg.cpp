
// AngoTimeDlg.cpp : ʵ���ļ�
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



CWinThread*			g_pthAlarm		=	NULL;
CWinThread*			g_pthSayTime	=	NULL;
CWinThread*			g_pthWork		=	NULL;
BOOL				g_bWork			=	TRUE;
DWORD				g_dwTasktype	=	0;
std::mutex			g_Mutex;

// CAngoTimeDlg �Ի���



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
	ON_WM_TIMER()			//��ʱ��
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
END_MESSAGE_MAP()


// CAngoTimeDlg ��Ϣ�������
void CAngoTimeDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	g_bWork = FALSE;
	

	g_Mutex.lock();
	g_pthAlarm->ResumeThread();
	g_pthSayTime->ResumeThread();
	g_pthWork->ResumeThread();
	g_Mutex.unlock();


	WaitForSingleObject(g_pthWork->m_hThread, INFINITE);
	WaitForSingleObject(g_pthAlarm->m_hThread, INFINITE);
	WaitForSingleObject(g_pthSayTime->m_hThread, INFINITE);
	


	CDialogEx::OnClose();
	CDialogEx::OnCancel();
}

BOOL CAngoTimeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//����������
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// Ψһʵ��
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, L"//AngoTime.exe");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// ��������Ѿ����ڲ����������� 
		// ������л������������ͷž������λ���������˳�����
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		//AngoMessageBox(_T("�����Ѿ�������"));
		CDialog::OnCancel();
	}

	InitClock();


	//����λ��
	CRect cr;
	GetClientRect(&cr);//��ȡ�Ի���ͻ������С
	ClientToScreen(&cr);//ת��ΪӫĻ����
	int x = GetSystemMetrics(SM_CXSCREEN);//��ȡӫĻ����Ŀ�ȣ���λΪ����
	int y = GetSystemMetrics(SM_CYSCREEN);//��ȡӫĻ����ĸ߶ȣ���λΪ����
	//MoveWindow((x-cr.Width() *2)/2 ,cr.top,cr.Width() *2,cr.Height() *2);//���Ͻ�

	MoveWindow(x - cr.Width(), cr.Height(), cr.Width(), cr.Height());

	OnViewShow();
	OnViewUp();
	
	InitThread();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAngoTimeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		OnTimer(m_uClock_Timer);
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAngoTimeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//����windows����������F1����
void CAngoTimeDlg::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	return;
	//CDialog::WinHelp(dwData, nCmd);
}

int CAngoTimeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	//֪ͨ����ͼ�꣬����ͼ��
	m_trayIcon.m_pCwnd = this;
	m_trayIcon.m_dwIconId = IDR_MAINFRAME;
	m_trayIcon.m_strTips = "�r(������)�q ";
	m_trayIcon.InitTrayIcon();


	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------
//�϶��ޱ���Ի��򴰿�
void CAngoTimeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

//�϶��ޱ���Ի��򴰿�
void CAngoTimeDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu* pmenu = m_popMenu.GetSubMenu(0);		//�����Ĳ˵�ʵ������IDR_MENU_POPUP�˵���ĳ����Ӳ˵��������ǵ�һ��
	CPoint pos;
	GetCursorPos(&pos);							//�����˵���λ�ã�����������ĵ�ǰλ��
	//��ʾ�ò˵�����һ������������ֵ�ֱ��ʾ�������ұ���ʾ����Ӧ����һ�
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
		//�ָ����ڻ�����С��
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->ShowWindow(SW_RESTORE);
		//����ò��ֻ��д����������ܱ�֤�ָ����ں󣬸ô��ڴ��ڻ״̬������ǰ�棩
	}
	else if (lparam == WM_RBUTTONDOWN)
	{
		//�����Ҽ��˵�
		CMenu* pmenu = m_popMenu.GetSubMenu(0);		//�����Ĳ˵�ʵ������IDR_MENU_POPUP�˵���ĳ����Ӳ˵��������ǵ�һ��
		CPoint pos;
		GetCursorPos(&pos);							//�����˵���λ�ã�����������ĵ�ǰλ��
		//��ʾ�ò˵�����һ������������ֵ�ֱ��ʾ�������ұ���ʾ����Ӧ����һ�
		pmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------------
void CAngoTimeDlg::InitClock()
{
	//���ÿ���ʱ���߶��Ĵ�����Ϊÿ��һ��
	m_uClock_Timer = this->SetTimer(1, 1000, NULL);

	//��Բ�ζԻ���
	CRgn  rgn;
	CRect  rc;
	GetClientRect(&rc);
	rgn.CreateEllipticRgn(rc.left, rc.top, rc.right-1, rc.bottom-1);
	SetWindowRgn(rgn, TRUE);
	rgn.DeleteObject();
	
	//��ʱ�ӱ���ͼƬ��Բ�ζԻ��򱳾�
	CBitmap   bm;
	bm.LoadBitmap(IDB_BMP_CLOCK);		//   ����ָ��bitmapͼƬ��·��   
	m_cBrush.CreatePatternBrush(&bm);
}

HBRUSH CAngoTimeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  �ڴ˸��� DC ���κ�����
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return m_cBrush;
}



void  CAngoTimeDlg::ClockTime()
{
	//��õ�ǰϵͳʱ�䡣
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

	
	//����Ϊ��ÿ����ǰ�ñ���ɫ��ȥ��һ�λ����루���ڱ���ɫ���䣬���Լ����˼��㣩
	//��ͼƬ��ȡ����ɫrgb
	//COLORREF cRgb = dc.GetPixel(m_Point_End);
	//BYTE byRed   = GetRValue(cRgb);
	//BYTE byGreen = GetGValue(cRgb);
	//BYTE byBlue	 = GetBValue(cRgb);

	//////////////////////////////////////////////	
	if ( !m_bFirstClock )
	{
		//�ñ���ɫ��ȥ��һ�λ�����
		cRgb = m_cLastHour;
		m_Point_End = m_point_lastHour;

		PenNew.CreatePen(PS_SOLID, 4, cRgb);
		BrushNew.CreateSolidBrush(cRgb);
		pBrushOld = dc.SelectObject(&BrushNew);
		pPenOld = dc.SelectObject(&PenNew);
		dc.MoveTo(m_Point_Start);
		dc.LineTo(m_Point_End);
	}

	//��ʱ��
	PenNew.DeleteObject();
	PenNew.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	pPenOld = dc.SelectObject(&PenNew);
	BrushNew.DeleteObject();
	BrushNew.CreateSolidBrush(RGB(0, 0, 0));
	pBrushOld = dc.SelectObject(&BrushNew);
	m_Point_End.x = 65 + LONG(22 * sin(H*PI / 30));
	m_Point_End.y = 64 - LONG(22 * cos(H*PI / 30));

	//����ԭ�е�rgb
	m_cLastHour = dc.GetPixel(m_Point_End);
	m_point_lastHour = m_Point_End;

	dc.MoveTo(m_Point_Start);
	dc.LineTo(m_Point_End);
	


	///////////////////////////////////////////////
	if ( !m_bFirstClock )
	{
		//�����ϴε�ָ��
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

	//������
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
	

	////////////////////////////////////////////	������Ķ���	
	S   = (S + 30) % 60;
	
	if ( !m_bFirstClock )
	{
		//�����ϴε�ָ��
		cRgb = m_cLastSecS;
		m_Point_End = m_point_lastSecS;

		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT, 2, cRgb);
		pPenOld = dc.SelectObject(&PenNew);
		dc.MoveTo(m_Point_Start);
		dc.LineTo(m_Point_End);
	}


	//���������
	PenNew.DeleteObject();
	PenNew.CreatePen(PS_DASHDOTDOT, 2, RGB(255, 0, 0));
	pPenOld = dc.SelectObject(&PenNew);
	m_Point_End.x = 65 + LONG(6 * sin(S*PI / 30));
	m_Point_End.y = 64 - LONG(6 * cos(S*PI / 30));

	m_cLastSecS = dc.GetPixel(m_Point_End);
	m_point_lastSecS = m_Point_End;

	dc.MoveTo(m_Point_Start);
	dc.LineTo(m_Point_End);
	


	///////////////////////������ĳ���
	S   = (S + 30) % 60;
	
	if ( !m_bFirstClock )
	{
		//�����ϴε�ָ��
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


	//�����볤��
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
}


//-------------------------------------------------------------------------------------------------------------------------



void CAngoTimeDlg::OnTimer(UINT_PTR nIDEvent)//����ʱ���߶�
{
	//�жϴ��ݹ�����ʱ�Ӵ������Ƿ����Լ������ʱ�Ӵ�����

	if (nIDEvent = m_uClock_Timer)
	{
		ClockTime();
	}
	else
	{

// 		for(int i=0;i<ringnum;i++)//�ж��Ƿ�������Ӧ��ִ��
// 		{
// 			if(time.GetHour()==mytimearray[i].hour&&time.GetMinute()==mytimearray[i].minute&&time.GetSecond()==mytimearray[i].second)
// 			{
// 				pThread2->ResumeThread();
// 				break;
// 			}
// 		}



// 		for(int i=0;i<tasknum;i++)//�ж��Ƿ��ж�ʱ����Ӧ��ִ��
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


// 		if(hoursound&&S==0&&M==0)//�ж��Ƿ����㱨ʱ
// 			SoundTime();
// 		//�ж��Ƿ��㱨ʱ
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
	//����ò��ֻ��д����������ܱ�֤�ָ����ں󣬸ô��ڴ��ڻ״̬������ǰ�棩
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);

	CMenu* pMenu = m_popMenu.GetSubMenu(0);
	//���õ�һ�㣺�����乴ѡ
	//pmenu->CheckMenuItem(ID_MENU_TOOL, MF_BYCOMMAND | MF_CHECKED);	//ͨ������ID��ѡ��ID_MENU_TOOL
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
	//���API����ʾ�Ƿ��Թ���Ա�������
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
void CAngoTimeDlg::InitThread()
{
	if ( g_pthAlarm || g_pthSayTime || g_pthWork )
		return;

	//������ʱ�߳�
	DWORD dwParam;
	g_pthAlarm = AfxBeginThread(ThProcAlarm, &dwParam, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED, 0);
	if (g_pthAlarm == NULL)
		AngoMessageBox(L"�����̴߳���!");


	//���������߳�
	DWORD dwParam2;
	g_pthSayTime = AfxBeginThread(ThProcSayTime, &dwParam2, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED, 0);
	if (g_pthSayTime == NULL)
		AngoMessageBox(L"�����̴߳���!");


	//��������ʱ�����߳�
	DWORD dwParam3;
	g_pthWork = AfxBeginThread(ThProcWork, &dwParam3, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED, 0);
	if (g_pthWork == NULL)
		AngoMessageBox(L"�����̴߳���!");

	
}

UINT ThProcAlarm(LPVOID pParam)
{
	CString strPath;
	while (g_bWork)
	{
		g_Mutex.lock();

		strPath = "c:\\win95\\media\\The Microsoft Sound.wav";
		PlaySound(strPath, NULL, SND_FILENAME | SND_ASYNC);
// 		PlaySound(MAKEINTRESOURCE(IDR_WAVESOUND), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
// 		Sleep(7000);
// 		PlaySound(MAKEINTRESOURCE(IDR_WAVESOUND), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
// 		Sleep(8000);
		
		g_Mutex.unlock();
	}
	
	TRACE("exit ThProcAlarm\n");
	return 0;
}
UINT ThProcSayTime(LPVOID pParam)
{
	while (g_bWork)
	{
		g_Mutex.lock();

		CTime m_NowTime;
		m_NowTime = CTime::GetCurrentTime();
		int n_Hour = m_NowTime.GetHour();
		//����Ϊ��ʱ����,ԭ��Ϊͨ���Ե�ǰʱ����ж�,���ò�ͬ�������ļ�(��Դ�ļ�)������ϲ���,�γɱ�ʱЧ��
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

		g_Mutex.unlock();
		g_pthSayTime->SuspendThread();

	}

	TRACE("exit ThProcSayTime\n");
	return 0;
}
UINT ThProcWork(LPVOID pParam)
{
	while (g_bWork)
	{
		TRACE("g_pthWork->SuspendThread()\n");
		Sleep(1000);
		g_pthWork->SuspendThread();
	}

	TRACE("exit ThProcWork\n");
	return 0;
}




void CAngoTimeDlg::OnSaytimeNow()
{
	if (g_pthSayTime)
	{
		g_pthSayTime->ResumeThread();
	}

}
