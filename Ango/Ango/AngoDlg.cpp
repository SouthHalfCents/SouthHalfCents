
// AngoDlg.cpp : ʵ���ļ�
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

// CAngoDlg �Ի���



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


// CAngoDlg ��Ϣ�������
//-------------------------------------------------------------------------------------------------------------------------
BOOL CAngoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	
	//����������
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// Ψһʵ��
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, L"Ango");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// ��������Ѿ����ڲ����������� 
		// ������л������������ͷž������λ���������˳�����
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		MessageBox(L"�����Ѿ�������");
		CDialog::OnCancel();
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


	CRect rtClient;
	GetWindowRect(rtClient);
	::SetWindowPos(m_hWnd, HWND_TOPMOST, rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), SWP_SHOWWINDOW);

	//RegAutoStart();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAngoDlg::OnPaint()
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
HCURSOR CAngoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//����windows����������F1����
void CAngoDlg::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	return;
	//CDialog::WinHelp(dwData, nCmd);
}

int CAngoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	//֪ͨ����ͼ�꣬����ͼ��
	m_trayIcon.m_pCwnd		=	this;
	m_trayIcon.m_dwIconId	=	IDR_MAINFRAME;
	m_trayIcon.m_strTips	=	"�r(������)�q ";
	m_trayIcon.InitTrayIcon();


	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------
//�϶��ޱ���Ի��򴰿�
void CAngoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

//�϶��ޱ���Ի��򴰿�
void CAngoDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU1);				//IDR_MENU_POPUP����ResourceView�д������༭��һ���˵�
	CMenu* pmenu = popMenu.GetSubMenu(0);		//�����Ĳ˵�ʵ������IDR_MENU_POPUP�˵���ĳ����Ӳ˵��������ǵ�һ��
	CPoint pos;
	GetCursorPos(&pos);							//�����˵���λ�ã�����������ĵ�ǰλ��
	//��ʾ�ò˵�����һ������������ֵ�ֱ��ʾ�������ұ���ʾ����Ӧ����һ�
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
		if (AngoMsgBox::MessageBox(strMsg, L"�˳�", MB_OKCANCEL) == IDOK)
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
		AngoMsgBox::MessageBox(strMsg,L"����", MB_POST_CENTER);
	}


	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CAngoDlg::OnOK()
{
	CString strMsg = L"�س���";
	AngoMsgBox::MessageBox(strMsg);

}

void CAngoDlg::OnCancel()
{
	CString strMsg;
	strMsg = "�Ƿ��˳�����";
	if( AngoMsgBox::MessageBox(strMsg,L"�˳�",MB_OKCANCEL) == IDCANCEL)
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
		//�ָ����ڻ�����С��
		if (m_trayIcon.m_bMin == TRUE)
		{
			AfxGetMainWnd()->ShowWindow(SW_SHOW);
			AfxGetMainWnd()->ShowWindow(SW_RESTORE);
			//����ò��ֻ��д����������ܱ�֤�ָ����ں󣬸ô��ڴ��ڻ״̬������ǰ�棩
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
		//�����Ҽ��˵�
		CMenu popMenu;
		popMenu.LoadMenu(IDR_MENU1);				//IDR_MENU_POPUP����ResourceView�д������༭��һ���˵�
		CMenu* pmenu = popMenu.GetSubMenu(0);		//�����Ĳ˵�ʵ������IDR_MENU_POPUP�˵���ĳ����Ӳ˵��������ǵ�һ��
		CPoint pos;
		GetCursorPos(&pos);							//�����˵���λ�ã�����������ĵ�ǰλ��
		//��ʾ�ò˵�����һ������������ֵ�ֱ��ʾ�������ұ���ʾ����Ӧ����һ�
		pmenu->TrackPopupMenu( TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
		
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------------
void CAngoDlg::OnUpLayer()
{
	//���������ϲ㣬ȡ��
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
	//�ָ����ڻ�����С��
	if (m_trayIcon.m_bMin == TRUE)
	{
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->ShowWindow(SW_RESTORE);
		//����ò��ֻ��д����������ܱ�֤�ָ����ں󣬸ô��ڴ��ڻ״̬������ǰ�棩
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
	strMsg = "˵��";
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



//����������
//���� wszPath[]����·���� wszName[]�½�����
BOOL CAngoDlg::RegAutoStart()
{
	// 32λ������64λϵͳ����,����ע����ض��������.


	HMODULE hModule = GetModuleHandle(NULL);

	//�õ�����������·��
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
		0,   //��������������Ϊ0
		NULL,//�������࣬ͬͨ��ΪNULL
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisposition  //���ز���
		);
	

	char data_Name[MAX_PATH] = "Ango";

	lRet = ::RegSetValueExA(
		hKey,
		data_Name,
		0,						//����
		REG_SZ,					//�ַ���
		(CONST BYTE *)szPath,	//��������
		(DWORD)strlen(szPath)
		);

	if (lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		OutputDebugStringA("��ע���ʧ��");
		return FALSE;
	}

	RegCloseKey(hKey);

#endif



	return TRUE;
}
//-------------------------------------------------------------------------------------------------------------------------


