
// YY520TimeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YY520Time.h"
#include "YY520TimeDlg.h"
#include "afxdialogex.h"


#include <afxinet.h>
#include <string>

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CYY520TimeDlg �Ի���


CYY520TimeDlg::CYY520TimeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CYY520TimeDlg::IDD, pParent)
	, m_uClock_Timer(0)
	, m_Point_Start(0)
	, m_Point_End(0)
{

	CUtils::DebugShow("��������");
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

CYY520TimeDlg::~CYY520TimeDlg()
{
	CUtils::DebugShow("�˳�����");
}

void CYY520TimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CYY520TimeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()			//��ʱ��
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()

	ON_MESSAGE(MAIN_WM_NOTIFYICON, &CYY520TimeDlg::OnNotifyIcon)
	ON_COMMAND(ID_MENU_EXIT, &CYY520TimeDlg::OnMenuExit)
	ON_COMMAND(ID_VIEW_UP, &CYY520TimeDlg::OnViewUp)
	ON_COMMAND(ID_VIEW_DOWN, &CYY520TimeDlg::OnViewDown)
	ON_COMMAND(ID_VIEW_SHOW, &CYY520TimeDlg::OnViewShow)
	ON_COMMAND(ID_VIEW_HIDE, &CYY520TimeDlg::OnViewHide)
	ON_COMMAND(ID_MENU_ANGO, &CYY520TimeDlg::OnMenuAngo)
	ON_WM_CLOSE()
	ON_COMMAND(ID_SAYTIME_NOW, &CYY520TimeDlg::OnSaytimeNow)
	ON_COMMAND(ID_SAYTIME_ALL, &CYY520TimeDlg::OnSaytimeAll)
	ON_COMMAND(ID_SAYTIME_HALF, &CYY520TimeDlg::OnSaytimeHalf)
	ON_COMMAND(ID_SAYTIME_CLOSE, &CYY520TimeDlg::OnSaytimeClose)
	ON_COMMAND(ID_CFG_AUTORUN, &CYY520TimeDlg::OnCfgAutoStart)
	ON_COMMAND(ID_CLOCK_ON, &CYY520TimeDlg::OnClockOn)
	ON_COMMAND(ID_CLOCK_OFF, &CYY520TimeDlg::OnClockOff)
	ON_COMMAND(ID_CLOCK_CONFIG, &CYY520TimeDlg::OnClockConfig)
	ON_COMMAND(ID_MENU_TASK, &CYY520TimeDlg::OnMenuTask)
	ON_COMMAND(ID_CFG_OTHER, &CYY520TimeDlg::OnCfgOther)
	ON_COMMAND(ID_MENU_WEATHER, &CYY520TimeDlg::OnMenuWeather)
	ON_COMMAND(ID_MENU_MAGIC_TIME, &CYY520TimeDlg::OnMenuMagicTime)
	ON_COMMAND(ID_MENU_LANTERN, &CYY520TimeDlg::OnMenuLantern)
END_MESSAGE_MAP()


// CYY520TimeDlg ��Ϣ�������
void CYY520TimeDlg::OnClose()
{
	StopWork();

	if (m_hOnlyMutex)
	{
		CloseHandle(m_hOnlyMutex);
		m_hOnlyMutex = NULL;
	}
	CDialogEx::OnClose();

}

BOOL CYY520TimeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//����������
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// Ψһʵ��
	m_hOnlyMutex = CreateMutex(NULL, FALSE, _T("//YY520Time.exe"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// ��������Ѿ����ڲ����������� ������л������������ͷž������λ���������˳�����
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
	OnViewUp();
	InitSettings();
	

	return TRUE;  
}





void CYY520TimeDlg::OnPaint()
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CYY520TimeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//����windows����������F1����
void CYY520TimeDlg::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	return;
	//CDialog::WinHelp(dwData, nCmd);
}

int CYY520TimeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//֪ͨ����ͼ�꣬����ͼ��
	m_trayIcon.m_pCwnd = this;
	m_trayIcon.m_dwIconId = IDR_MAINFRAME;
	m_trayIcon.m_strTips = "����o(*�R���Q)��";
	m_trayIcon.InitTrayIcon();
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------
//�϶��ޱ���Ի��򴰿�
void CYY520TimeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

//�϶��ޱ���Ի��򴰿�
void CYY520TimeDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu* pmenu = m_popMenu.GetSubMenu(0);		//�����Ĳ˵�ʵ������IDR_MENU_POPUP�˵���ĳ����Ӳ˵��������ǵ�һ��
	CPoint pos;
	GetCursorPos(&pos);							//�����˵���λ�ã�����������ĵ�ǰλ��
	//��ʾ�ò˵�����һ������������ֵ�ֱ��ʾ�������ұ���ʾ����Ӧ����һ�
	pmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
	CDialogEx::OnRButtonDown(nFlags, point);
}

void CYY520TimeDlg::OnOK()
{
}

void CYY520TimeDlg::OnCancel()
{
	//OnClose();
	//CDialogEx::OnCancel();
}

//-------------------------------------------------------------------------------------------------------------------------
LRESULT CYY520TimeDlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
{
	if (lparam == WM_LBUTTONDOWN)
	{
		//�ָ����ڻ�����С��
		//AfxGetMainWnd()->ShowWindow(SW_SHOW);
		//AfxGetMainWnd()->ShowWindow(SW_RESTORE);
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
void CYY520TimeDlg::InitPosition()
{
	//����λ��
	CRect cr;
	GetClientRect(&cr);//��ȡ�Ի���ͻ������С
	ClientToScreen(&cr);//ת��ΪӫĻ����
	int x = GetSystemMetrics(SM_CXSCREEN);//��ȡӫĻ����Ŀ�ȣ���λΪ����
	int y = GetSystemMetrics(SM_CYSCREEN);//��ȡӫĻ����ĸ߶ȣ���λΪ����
	//MoveWindow((x-cr.Width() *2)/2 ,cr.top,cr.Width() *2,cr.Height() *2);//���Ͻ�
	MoveWindow(x - cr.Width(), cr.Height(), cr.Width(), cr.Height());
}
void CYY520TimeDlg::InitClock()
{
	//���ÿ���ʱ���߶��Ĵ�����Ϊÿ��һ��
	m_uClock_Timer = this->SetTimer(1, 1000, NULL);

	//��Բ�ζԻ���
	CRgn  rgn;
	CRect  rc;
	GetClientRect(&rc);
	rgn.CreateEllipticRgn(rc.left+2, rc.top+2, rc.right-3, rc.bottom-3);
	SetWindowRgn(rgn, TRUE);
	rgn.DeleteObject();
	
	//��ʱ�ӱ���ͼƬ��Բ�ζԻ��򱳾�
	CBitmap   bm;
	bm.LoadBitmap(IDB_BMP_CLOCK);		//   ����ָ��bitmapͼƬ��·��   
	BOOL bRet = m_cBrush.CreatePatternBrush(&bm);
}

HBRUSH CYY520TimeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  �ڴ˸��� DC ���κ�����
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return m_cBrush;
}

void CYY520TimeDlg::InitRgbMap()
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

	// ��ȡλͼ����  
	HDC hdc = ::GetDC(NULL);
	BYTE* pBits = (BYTE*)new BYTE[m_bmpClock.bmWidth * m_bmpClock.bmHeight * nbyte];
	::ZeroMemory(pBits, m_bmpClock.bmWidth * m_bmpClock.bmHeight * nbyte);
	if (!::GetDIBits(hdc, bitmap, 0, m_bmpClock.bmHeight, pBits, &bi, DIB_RGB_COLORS))
	{
		delete pBits;
		pBits = NULL;
		AngoBox.AngoMessageBox(_T("��ʼ��ʱ��λͼʧ��"));
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
void CYY520TimeDlg::OnTimer(UINT_PTR nIDEvent)//����ʱ���߶�
{
	//�жϴ��ݹ�����ʱ�Ӵ������Ƿ����Լ������ʱ�Ӵ�����

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

void CYY520TimeDlg::OnViewUp()
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


void CYY520TimeDlg::OnViewDown()
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


void CYY520TimeDlg::OnViewShow()
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


void CYY520TimeDlg::OnViewHide()
{
	//AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
	/*����Ի�����ʾ��������������С��֮��������һ����С��һ����ʾ�������ϡ���ʱ��������ڵ����ͻ����WM_PAINT��Ϣ*/

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


void CYY520TimeDlg::OnMenuAngo()
{
	BOOL bAdmin = CUtils::UpPrivilege();
	CString strPath = CUtils::GetAppDir();
	strPath += _T("\\YY520.exe");
	//���API����ʾ�Ƿ��Թ���Ա�������
	CUtils::ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL,TRUE);

}

void CYY520TimeDlg::OnMenuExit()
{
	OnClose();
	CDialogEx::OnCancel();
}






void CYY520TimeDlg::OnSaytimeNow()
{
	ReleaseSemaphore(g_hSemaph_SayTime, 1, NULL);	//�ͷ�1���ź���
}

BOOL CYY520TimeDlg::SetSayTimeState()
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

void CYY520TimeDlg::OnSaytimeAll()
{
	m_nSayTime = SAYTIME_ALL;
	if (SetSayTimeState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, SAYTIME_ENTRY, m_nSayTime);
}


void CYY520TimeDlg::OnSaytimeHalf()
{
	m_nSayTime = SAYTIME_HALF;
	if (SetSayTimeState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, SAYTIME_ENTRY, m_nSayTime);
}


void CYY520TimeDlg::OnSaytimeClose()
{
	m_nSayTime = SAYTIME_CLOSE;
	if (SetSayTimeState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, SAYTIME_ENTRY, m_nSayTime);
}

BOOL CYY520TimeDlg::SetAutoStart()
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

	CString strKeyName = _T("YY520Time");
	CString strKeyValue = CUtils::GetAppPath();

	BOOL bResult = CUtils::SetRegAutoStart(TRUE, strKeyName, strKeyValue);
	return bResult;
}

void CYY520TimeDlg::OnCfgAutoStart()
{
	m_bAutoRun = !m_bAutoRun;
	if (SetAutoStart() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, AUTORUN_ENTRY, m_bAutoRun);
}


void CYY520TimeDlg::OnClockOn()
{
	m_bClockOn = TRUE;
	if (SetClockState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, CLOCK_ENTRY, m_bClockOn);
}


void CYY520TimeDlg::OnClockOff()
{
	m_bClockOn = FALSE;
	if (SetClockState() == FALSE)
	{
		return;
	}
	AfxGetApp()->WriteProfileInt(ANGO_SECTION_SET, CLOCK_ENTRY, m_bClockOn);
}

BOOL CYY520TimeDlg::SetClockState()
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

void CYY520TimeDlg::OnClockConfig()
{
	CClock clock_dlg;
	if (IDOK == clock_dlg.DoModal())
	{

	}
}


void CYY520TimeDlg::OnMenuTask()
{
	CTask task_dlg;
	if (IDOK == task_dlg.DoModal())
	{

	}
}

void CYY520TimeDlg::OnCfgOther()
{
	CConfig config_dlg;
	if (IDOK == config_dlg.DoModal())
	{

	}
}

//---------------------------------------------------------------------------------------------------------------------
void CYY520TimeDlg::InitSettings()
{
	//��������
	m_bClockOn = AfxGetApp()->GetProfileInt(ANGO_SECTION_SET, CLOCK_ENTRY, 0);
	SetClockState();

	//��ʱ����
	m_nSayTime = AfxGetApp()->GetProfileInt(ANGO_SECTION_SET, SAYTIME_ENTRY, 0);
	SetSayTimeState();

	//������������
	m_bAutoRun = AfxGetApp()->GetProfileInt(ANGO_SECTION_SET, AUTORUN_ENTRY, 0);
	SetAutoStart();

	//AfxGetApp()->GetProfileString(ANGO_SECTION_SET, AUTORUN_ENTRY, 0);
}
//---------------------------------------------------------------------------------------------------------------------





void CYY520TimeDlg::OnMenuWeather()
{
	CString strPath = CUtils::GetAppDir();
	strPath += _T("\\AngoWeather.exe");
	//���API����ʾ�Ƿ��Թ���Ա�������
	
	CUtils::ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL,TRUE);
}

//��ýʱ��
void CYY520TimeDlg::OnMenuMagicTime()
{
	CString strPath = CUtils::GetAppDir();
	strPath += _T("\\Tools\\mytime\\mytime.exe");
	//���API����ʾ�Ƿ��Թ���Ա�������

	CUtils::ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL,TRUE);
}

//�������Ʒ�ǽ
void CYY520TimeDlg::OnMenuLantern()
{
	TCHAR szUser[MAX_PATH] = {0};
	DWORD nlen = MAX_PATH;
	GetUserName(szUser, &nlen);

	TCHAR szBuf[8192] = {0};
	_sntprintf(szBuf, 8192, _T("C:\\Users\\%s\\AppData\\Roaming\\Lantern\\lantern.exe"), szUser);

	CUtils::ShellExecute(NULL, _T("open"), szBuf, NULL, NULL, SW_SHOWNORMAL,TRUE);
}
