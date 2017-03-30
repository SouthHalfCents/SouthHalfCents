
// AngoMsgBoxTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AngoMsgBoxTest.h"
#include "AngoMsgBoxTestDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAngoMsgBoxTestDlg �Ի���



CAngoMsgBoxTestDlg::CAngoMsgBoxTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngoMsgBoxTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAngoMsgBoxTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAngoMsgBoxTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CAngoMsgBoxTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAngoMsgBoxTestDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAngoMsgBoxTestDlg ��Ϣ�������

BOOL CAngoMsgBoxTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAngoMsgBoxTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAngoMsgBoxTestDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAngoMsgBoxTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAngoMsgBoxTestDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();

// 	TCHAR szLocalPath[MAX_PATH];
// 	GetModuleFileName(0, szLocalPath, MAX_PATH);
// 
// 	TCHAR szFullPath[4096] = { 0 };
// 	GetFullPathName(szLocalPath, 4096, szFullPath, NULL);
// 
// 	CString strTemp = szFullPath;
// 	strTemp = strTemp.Left(strTemp.ReverseFind('\\'));
// 	strTemp += _T("\\AngoMsgBox.dll");
// 
// 	HMODULE hDll = LoadLibrary(strTemp);
// 	if (!hDll)
// 		return;

	//typedef int( *pfnGetModuleVer)(void);
	//typedef TCHAR* ( *pfnGetModuleName)(void);
	//typedef int( *pfgnMessageBox)(CString strMsg, CString strTitle = _T(""), UINT nType = MB_POST_BESIDE | MB_OK );

	//pfnGetModuleVer  GetModuleVer = (pfnGetModuleVer)GetProcAddress(hDll, "GetModuleVer");
	//pfnGetModuleName GetModuleName = (pfnGetModuleName)GetProcAddress(hDll, "GetModuleName");
	//pfgnMessageBox	 AngoMessageBox = (pfgnMessageBox)GetProcAddress(hDll, "AngoMessageBox");

	//int nId = GetModuleVer();
	//CString strName = GetModuleName();

	//CString strMsg;
	//strMsg.Format(_T("%d"), nId);
	//AngoMessageBox(strMsg,_T(""),0);
	//AngoMessageBox(strName,_T(""),0);
	//FreeLibrary(hDll);

	AngoBox.AngoMessageBox(_T("666"));
	AngoBox.AngoMessageBox(_T("666"), _T("123"), MB_YESNO);
}


void CAngoMsgBoxTestDlg::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}
