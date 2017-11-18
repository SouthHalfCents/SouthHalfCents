
// YY520Dlg.cpp : ʵ���ļ�
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

// CYY520Dlg �Ի���



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


// CYY520Dlg ��Ϣ�������
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	
	//����������
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// Ψһʵ��
	m_hOnlyMutex = CreateMutex(NULL, FALSE, _T("//YY520.exe"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// ��������Ѿ����ڲ�����������  ������л������������ͷž������λ���������˳�����
		CloseHandle(m_hOnlyMutex);
		m_hOnlyMutex = NULL;
		//AngoBox.AngoMessageBox(_T("�����Ѿ�������"));
		CDialog::OnCancel();
		return TRUE;
	}



	//�Զ���Ի������
	vector<DWORD> vData;
	vData.push_back(IDB_BITMAP_MAIN_1);
	vData.push_back(IDB_BITMAP_MAIN_2);
	m_cusView.m_pCwnd = this;
	m_cusView.InitBmpVector(vData);
	m_cusView.InitCusView();


	//����λ��
	CRect cr;
	GetClientRect(&cr);//��ȡ�Ի���ͻ������С
	ClientToScreen(&cr);//ת��ΪӫĻ����
	int x = GetSystemMetrics(SM_CXSCREEN);//��ȡӫĻ����Ŀ�ȣ���λΪ����
	int y = GetSystemMetrics(SM_CYSCREEN);//��ȡӫĻ����ĸ߶ȣ���λΪ����
	//MoveWindow((x-cr.Width() *2)/2 ,cr.top,cr.Width() *2,cr.Height() *2);//���Ͻ�
	MoveWindow(x-cr.Width()+150 ,y-cr.Height()+150,cr.Width(),cr.Height());
	//MoveWindow(x-cr.Width() ,y-cr.Height(),cr.Width(),cr.Height());

	OnMenuShow();
	OnMenuUp();
	//RegAutoStart();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CYY520Dlg::OnPaint()
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
		//�Զ���Ի������
		m_cusView.OnPaint();
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CYY520Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//����windows����������F1����
void CYY520Dlg::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	return;
	//CDialog::WinHelp(dwData, nCmd);
}

int CYY520Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	//֪ͨ����ͼ�꣬����ͼ��
	m_trayIcon.m_pCwnd		=	this;
	m_trayIcon.m_dwIconId	=	IDR_MAINFRAME;
	m_trayIcon.m_strTips	=	"���㣨�ţ�3������";
	m_trayIcon.InitTrayIcon();


	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------
//�϶��ޱ���Ի��򴰿�
void CYY520Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

//�϶��ޱ���Ի��򴰿�
void CYY520Dlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu* pmenu = m_popMenu.GetSubMenu(0);		//�����Ĳ˵�ʵ������IDR_MENU_POPUP�˵���ĳ����Ӳ˵��������ǵ�һ��
	CPoint pos;
	GetCursorPos(&pos);							//�����˵���λ�ã�����������ĵ�ǰλ��
	//��ʾ�ò˵�����һ������������ֵ�ֱ��ʾ�������ұ���ʾ����Ӧ����һ�
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
		strMsg = "�س���";
	}
	break;

	case 16:
	{
		m_cusView.SetNextView();
		OnPaint();
		//MessageBox(L"Shift��");
	}
	break;

	case 27:
	{
		strMsg = "�Ƿ��˳�����";
		if (AngoBox.AngoMessageBox(strMsg, L"�˳�", MB_OKCANCEL) == IDOK)
		{
			CDialog::OnCancel();
		}
	}
	break;

	case 112:
	{
		strMsg = "F1��";
	}
	break;

	case 113:
	{
		strMsg = "F2��";
	}
	break;

	case 114:
	{
		strMsg = "F3��";
	}
	break;

	case 115:
	{
		strMsg = "F4��";
	}
	break;

	case 116:
	{
		strMsg = "F5��";
	}
	break;

	case 117:
	{
		strMsg = "F6��";
	}
	break;
	default:
		strMsg = "������";
	}

	if (strMsg.GetLength())
	{
		AngoBox.AngoMessageBox(strMsg,L"����", MB_POST_CENTER);
	}


	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CYY520Dlg::OnOK()
{
	CString strMsg = L"�س���";
	AngoBox.AngoMessageBox(strMsg);

}

void CYY520Dlg::OnCancel()
{
	CString strMsg;
	strMsg = "�Ƿ��˳�����";
	//if( AngoBox.AngoMessageBox(strMsg,L"�˳�",MB_OKCANCEL) == IDCANCEL)
	CDialog::OnCancel();
}

//-------------------------------------------------------------------------------------------------------------------------
LRESULT CYY520Dlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
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
		pmenu->TrackPopupMenu( TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------------




void CYY520Dlg::OnMenuAbout()
{
	CString strMsg;
	strMsg = "˵��";
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



//����������
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
	AngoBox.AngoMessageBox(_T("������"));
}

void CYY520Dlg::OnMenuShow()
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
			pMenu->CheckMenuItem(ID_MENU_SHOW, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(ID_MENU_HIDE, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}
}

void CYY520Dlg::OnMenuHide()
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
	//���API����ʾ�Ƿ��Թ���Ա�������
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
