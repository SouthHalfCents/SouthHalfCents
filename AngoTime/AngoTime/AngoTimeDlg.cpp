
// AngoTimeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AngoTime.h"
#include "AngoTimeDlg.h"
#include "afxdialogex.h"
#include "MsgBoxEx.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif





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

	m_nSayTime		=	SAYTIME_CLOSE;
	m_bClockState	=	FALSE;
	m_bAutoRun		=	FALSE;

	m_hThread_Clock	=	NULL;
	m_hSemaph_Clock	=	NULL;

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
	ON_COMMAND(ID_SAYTIME_ALL, &CAngoTimeDlg::OnSaytimeAll)
	ON_COMMAND(ID_SAYTIME_HALF, &CAngoTimeDlg::OnSaytimeHalf)
	ON_COMMAND(ID_SAYTIME_CLOSE, &CAngoTimeDlg::OnSaytimeClose)
	ON_COMMAND(ID_CFG_AUTORUN, &CAngoTimeDlg::OnCfgAutorun)
	ON_COMMAND(ID_CLOCK_ON, &CAngoTimeDlg::OnClockOn)
	ON_COMMAND(ID_CLOCK_OFF, &CAngoTimeDlg::OnClockOff)
	ON_COMMAND(ID_CLOCK_CONFIG, &CAngoTimeDlg::OnClockConfig)
	ON_COMMAND(ID_MENU_TASK, &CAngoTimeDlg::OnMenuTask)
	ON_COMMAND(ID_CFG_OTHER, &CAngoTimeDlg::OnCfgOther)
END_MESSAGE_MAP()


// CAngoTimeDlg ��Ϣ�������
void CAngoTimeDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	g_bWork = FALSE;
	ReleaseSemaphore(m_hSemaph_Clock, 1, NULL);		//�ͷ�1���ź���
	WaitForSingleObject(m_hThread_Clock, INFINITE);
	CloseHandle(m_hSemaph_Clock);
	CloseHandle(m_hThread_Clock);

	CleanThread();


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

	InitThread();
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
	InitSettings();
	

	
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
	BOOL bRet = m_cBrush.CreatePatternBrush(&bm);

	InitRgbMap();

	//�����߳�
	m_hSemaph_Clock	= CreateSemaphore(NULL, 0, 2, _T("AngoTime_Clock"));
	m_hThread_Clock	= (HANDLE)_beginthreadex(NULL, 0, Thread_Clock, this, 0, NULL);
}

HBRUSH CAngoTimeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  �ڴ˸��� DC ���κ�����
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return m_cBrush;
}

void CAngoTimeDlg::InitRgbMap()
{
	

	CClientDC dc(this);
	CPoint cPos(65,64);

	CBitmap bitmap;
	BOOL bLoad = bitmap.LoadBitmap(IDB_BMP_CLOCK);
	CSize csize = bitmap.GetBitmapDimension();

	COLORREF C = dc.GetPixel(cPos);
	int R = GetRValue(C);
	int G = GetGValue(C);
	int B = GetBValue(C);

	m_mapPointRgb[cPos] = C;


	BITMAP bm;
	bitmap.GetBitmap(&bm);

	int nbyte = bm.bmBitsPixel / 8;

	BITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = bm.bmWidth;
	bi.bmiHeader.biHeight = -bm.bmHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = bm.bmBitsPixel;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = bm.bmWidth * bm.bmHeight * nbyte;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	// ��ȡλͼ����  
	HDC hdc = ::GetDC(NULL);
	BYTE* pBits = (BYTE*)new BYTE[bm.bmWidth * bm.bmHeight * nbyte];
	::ZeroMemory(pBits, bm.bmWidth * bm.bmHeight * nbyte);
	if (!::GetDIBits(hdc, bitmap, 0, bm.bmHeight, pBits, &bi, DIB_RGB_COLORS))
	{
		delete pBits;
		pBits = NULL;
	}
	for (int i = 0; i < bm.bmWidth; ++i)
	{
		for (int j = 0; j < bm.bmHeight; ++j)
		{
			BYTE r = pBits[i * nbyte + j * bm.bmWidthBytes + 2];
			BYTE g = pBits[i * nbyte + j * bm.bmWidthBytes + 1];
			BYTE b = pBits[i * nbyte + j * bm.bmWidthBytes + 0];
			//����Ϳ��������Ǵ�����
		}
	}

	delete pBits;
	pBits = NULL;

}

unsigned int __stdcall  Thread_Clock(LPVOID pParam)
{
	CAngoTimeDlg* pBase = (CAngoTimeDlg*)pParam;

	//��õ�ǰϵͳʱ�䡣
	CTime tTime = CTime::GetCurrentTime();
	int   S = 0;
	float M = 0;
	float H = 0;

	pBase->m_Point_Start.x = 65;
	pBase->m_Point_Start.y = 64;
	COLORREF cRgb	= 0;

	CPen *pPenOld = NULL;
	CBrush *pBrushOld = NULL;
	CPen PenNew;
	CBrush BrushNew;
	CClientDC dc(pBase);

	while (g_bWork)
	{ 
		WaitForSingleObject(pBase->m_hSemaph_Clock,INFINITE);

		tTime = CTime::GetCurrentTime();
		S = tTime.GetSecond();
		M = float(tTime.GetMinute() + S / 60.0);
		H = float(tTime.GetHour() + M / 60.0);
		if (H > 12)
			H = H - 12;
		H = H * 5;


		//����Ϊ��ÿ����ǰ�ñ���ɫ��ȥ��һ�λ����루���ڱ���ɫ���䣬���Լ����˼��㣩
		//��ͼƬ��ȡ����ɫrgb
		//COLORREF cRgb = dc.GetPixel(m_Point_End);
		//BYTE byRed   = GetRValue(cRgb);
		//BYTE byGreen = GetGValue(cRgb);
		//BYTE byBlue	 = GetBValue(cRgb);

		//////////////////////////////////////////////	
		if ( !pBase->m_bFirstClock )
		{
			//�ñ���ɫ��ȥ��һ�λ�����
			cRgb = pBase->m_cLastHour;
			pBase->m_Point_End = pBase->m_point_lastHour;

			PenNew.CreatePen(PS_SOLID, 4, cRgb);
			BrushNew.CreateSolidBrush(cRgb);
			pBrushOld = dc.SelectObject(&BrushNew);
			pPenOld = dc.SelectObject(&PenNew);
			dc.MoveTo(pBase->m_Point_Start);
			dc.LineTo(pBase->m_Point_End);
		}

		//��ʱ��
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
		pPenOld = dc.SelectObject(&PenNew);
		BrushNew.DeleteObject();
		BrushNew.CreateSolidBrush(RGB(0, 0, 0));
		pBrushOld = dc.SelectObject(&BrushNew);
		pBase->m_Point_End.x = 65 + LONG(22 * sin(H*PI / 30));
		pBase->m_Point_End.y = 64 - LONG(22 * cos(H*PI / 30));

		//����ԭ�е�rgb
		pBase->m_cLastHour = dc.GetPixel(pBase->m_Point_End);
		pBase->m_point_lastHour = pBase->m_Point_End;

		dc.MoveTo(pBase->m_Point_Start);
		dc.LineTo(pBase->m_Point_End);
	


		///////////////////////////////////////////////
		if ( !pBase->m_bFirstClock )
		{
			//�����ϴε�ָ��
			cRgb = pBase->m_cLastMin;
			pBase->m_Point_End = pBase->m_point_lastMin;

			BrushNew.DeleteObject();
			BrushNew.CreateSolidBrush(cRgb);
			pBrushOld = dc.SelectObject(&BrushNew);
			PenNew.DeleteObject();
			PenNew.CreatePen(PS_SOLID, 3, cRgb);
			pPenOld = dc.SelectObject(&PenNew);
			dc.MoveTo(pBase->m_Point_Start);
			dc.LineTo(pBase->m_Point_End);
		}

		//������
		BrushNew.DeleteObject();
		BrushNew.CreateSolidBrush(RGB(0, 0, 0));
		pBrushOld = dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		pPenOld = dc.SelectObject(&PenNew);
		pBase->m_Point_End.x = 65 + LONG(30 * sin(M*PI / 30));
		pBase->m_Point_End.y = 64 - LONG(30 * cos(M*PI / 30));

		pBase->m_cLastMin = dc.GetPixel(pBase->m_Point_End);
		pBase->m_point_lastMin = pBase->m_Point_End;

		dc.MoveTo(pBase->m_Point_Start);
		dc.LineTo(pBase->m_Point_End);
	

		////////////////////////////////////////////	������Ķ���	
		S   = (S + 30) % 60;
	
		if ( !pBase->m_bFirstClock )
		{
			//�����ϴε�ָ��
			cRgb = pBase->m_cLastSecS;
			pBase->m_Point_End = pBase->m_point_lastSecS;

			PenNew.DeleteObject();
			PenNew.CreatePen(PS_DASHDOTDOT, 2, cRgb);
			pPenOld = dc.SelectObject(&PenNew);
			dc.MoveTo(pBase->m_Point_Start);
			dc.LineTo(pBase->m_Point_End);
		}


		//���������
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT, 2, RGB(255, 0, 0));
		pPenOld = dc.SelectObject(&PenNew);
		pBase->m_Point_End.x = 65 + LONG(6 * sin(S*PI / 30));
		pBase->m_Point_End.y = 64 - LONG(6 * cos(S*PI / 30));

		pBase->m_cLastSecS = dc.GetPixel(pBase->m_Point_End);
		pBase->m_point_lastSecS = pBase->m_Point_End;

		dc.MoveTo(pBase->m_Point_Start);
		dc.LineTo(pBase->m_Point_End);
	


		///////////////////////������ĳ���
		S   = (S + 30) % 60;
	
		if ( !pBase->m_bFirstClock )
		{
			//�����ϴε�ָ��
			cRgb = pBase->m_cLastSecL;
			pBase->m_Point_End = pBase->m_point_lastSecL;

			//		BrushNew.DeleteObject();
			//		BrushNew.CreateSolidBrush(RGB(C,C,C));
			//		BrushOld=dc.SelectObject(&BrushNew);
			PenNew.DeleteObject();
			PenNew.CreatePen(PS_DASHDOTDOT, 2, cRgb);
			pPenOld = dc.SelectObject(&PenNew);
			dc.MoveTo(pBase->m_Point_Start);
			dc.LineTo(pBase->m_Point_End);
		}


		//�����볤��
		//		BrushNew.DeleteObject();
		//		BrushNew.CreateSolidBrush(RGB(255,0,0));
		//		BrushOld=dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT, 2, RGB(255, 0, 0));
		pPenOld = dc.SelectObject(&PenNew);
		pBase->m_Point_End.x = 65 + LONG(30 * sin(S*PI / 30));
		pBase->m_Point_End.y = 64 - LONG(30 * cos(S*PI / 30));

		pBase->m_cLastSecL = dc.GetPixel(pBase->m_Point_End);
		pBase->m_point_lastSecL = pBase->m_Point_End;

		dc.MoveTo(pBase->m_Point_Start);
		dc.LineTo(pBase->m_Point_End);


		PenNew.DeleteObject();
		BrushNew.DeleteObject();
		//////////////////////////////////////////////////////////////////
		pBase->m_bFirstClock = FALSE;
	
		dc.SetPixel(pBase->m_Point_Start, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x + 1, pBase->m_Point_Start.y,		RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x,		pBase->m_Point_Start.y + 1, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x + 1, pBase->m_Point_Start.y + 1, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x - 1, pBase->m_Point_Start.y,		RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x,		pBase->m_Point_Start.y - 1, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x - 1, pBase->m_Point_Start.y - 1, RGB(0, 0, 0));

		//��ʱ
		if (!g_bWork)	
		{
			break;
		}
		if (pBase->m_nSayTime==SAYTIME_ALL &&S == 0 && M == 0)//�ж��Ƿ����㱨ʱ
			pBase->OnSaytimeNow();

		if (pBase->m_nSayTime==SAYTIME_HALF &&S == 0 && (M == 0 || M == 30))
			pBase->OnSaytimeNow();

		//��ʱ����
		if (!g_bWork)	
		{
			break;
		}
		CTask::CheckTask(tTime);

	}
	return 0;

}


//-------------------------------------------------------------------------------------------------------------------------



void CAngoTimeDlg::OnTimer(UINT_PTR nIDEvent)//����ʱ���߶�
{
	//�жϴ��ݹ�����ʱ�Ӵ������Ƿ����Լ������ʱ�Ӵ�����

	if (nIDEvent = m_uClock_Timer)
	{
		ReleaseSemaphore(m_hSemaph_Clock, 1, NULL);
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






void CAngoTimeDlg::OnSaytimeNow()
{
	ReleaseSemaphore(g_hSemaph_SayTime, 1, NULL);	//�ͷ�1���ź���
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
	//��������...

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
		0,   //��������������Ϊ0
		NULL,//�������࣬ͬͨ��ΪNULL
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisposition  //���ز���
		);


	char data_Name[MAX_PATH] = "Ango";
	if (m_bAutoRun)
	{
		lRet = ::RegSetValueExA(
			hKey,
			data_Name,
			0,						//����
			REG_SZ,					//�ַ���
			(CONST BYTE *)szPath,	//��������
			(DWORD)strlen(szPath)
			);
	}
	else
	{
		lRet = ::RegDeleteValueA(hKey,data_Name);   
	}


	if (lRet != ERROR_SUCCESS)
	{	
		OutputDebugStringA("��ע���ʧ��");
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
	//��������
	m_bClockState = AfxGetApp()->GetProfileInt(ANGO_SECTION, CLOCK_ENTRY, 0);
	if (m_bClockState)
	{
		OnClockOn();
	}
	else
	{
		OnClockOff();
	}


	//��ʱ����
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

	//������������
	m_bAutoRun = AfxGetApp()->GetProfileInt(ANGO_SECTION, AUTORUN_ENTRY, 0);
	m_bAutoRun = !m_bAutoRun;
	OnCfgAutorun();

}
//---------------------------------------------------------------------------------------------------------------------



