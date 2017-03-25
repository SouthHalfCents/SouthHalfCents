
// AngoWeatherDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AngoWeather.h"
#include "AngoWeatherDlg.h"
#include "afxdialogex.h"

#include "RenderEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LPCTSTR weatherTxt[] =
{
	TEXT("��"), TEXT("����"), TEXT("��"), TEXT("����"), TEXT("������"), TEXT("�����겢���б���"), TEXT("���ѩ"), TEXT("С��"), TEXT("����"), TEXT("����"), TEXT("����"),
	TEXT("����"), TEXT("�ش���"), TEXT("��ѩ"), TEXT("Сѩ"), TEXT("��ѩ"), TEXT("��ѩ"), TEXT("��ѩ"), TEXT("��"), TEXT("����"), TEXT("ɳ����"), TEXT("С��-����"),
	TEXT("����-����"), TEXT("����-����"), TEXT("����-����"), TEXT("����-�ش���"), TEXT("Сѩ-��ѩ"), TEXT("��ѩ-��ѩ"), TEXT("��ѩ-��ѩ"), TEXT("����"),
	TEXT("��ɳ"), TEXT("ǿɳ����"), TEXT("�"), TEXT("�����"), TEXT("���ߴ�ѩ"), TEXT("����"), TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT(""),
	TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT(""), TEXT("��")
};

LPCTSTR windDir[] =
{
	TEXT(""), TEXT("������"), TEXT("����"), TEXT("���Ϸ�"), TEXT("�Ϸ�"), TEXT("���Ϸ�"), TEXT("����"), TEXT("������"), TEXT("����"), TEXT("��ת����")
};

LPCTSTR windPower[] =
{
	TEXT("΢��"), TEXT("3-4��"), TEXT("4-5��"), TEXT("5-6��"), TEXT("6-7��"), TEXT("7-8��"), TEXT("8-9��"), TEXT("9-10��"), TEXT("10-11��"), TEXT("11-12��")
};


// CAngoWeatherDlg �Ի���



CAngoWeatherDlg::CAngoWeatherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngoWeatherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pParseJson = NULL;
	memset(&m_Weather, -1, sizeof m_Weather);
	m_Weather.szCity[0] = 0;
	m_bLoadFinished = false;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_popMenu.LoadMenu(IDR_MENU_RBTN);
}
CAngoWeatherDlg::~CAngoWeatherDlg()
{
}

void CAngoWeatherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAngoWeatherDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(MAIN_WM_NOTIFYICON, &CAngoWeatherDlg::OnNotifyIcon)

	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_CREATE()

	ON_COMMAND(ID_MENU_SHOW, &CAngoWeatherDlg::OnMenuShow)
	ON_COMMAND(ID_MENU_HIDE, &CAngoWeatherDlg::OnMenuHide)
	ON_COMMAND(ID_MENU_UP, &CAngoWeatherDlg::OnMenuUp)
	ON_COMMAND(ID_MENU_DOWN, &CAngoWeatherDlg::OnMenuDown)
	ON_COMMAND(ID_MENU_CITY_OPT, &CAngoWeatherDlg::OnMenuCityOpt)
	ON_COMMAND(ID_MENU_PREVIEW, &CAngoWeatherDlg::OnMenuPreview)
	ON_COMMAND(ID_MENU_ANGO, &CAngoWeatherDlg::OnMenuAngo)
	ON_COMMAND(ID_MENU_CONFIG, &CAngoWeatherDlg::OnMenuConfig)
	ON_COMMAND(ID_MENU_EXIT, &CAngoWeatherDlg::OnMenuExit)
	
END_MESSAGE_MAP()


// CAngoWeatherDlg ��Ϣ�������

BOOL CAngoWeatherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//����������
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// Ψһʵ��
	m_hOnlyMutex = CreateMutex(NULL, FALSE, L"//AngoWeather.exe");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// ��������Ѿ����ڲ����������� ������л������������ͷž������λ���������˳�����
		CloseHandle(m_hOnlyMutex);
		m_hOnlyMutex = NULL;
		OutputDebugString(_T("AngoWeather is already exit!\n"));
		CDialog::OnCancel();
		return TRUE;
	}


	m_pParseJson = CParseJson::GetInstance();

	m_pImageBack.LoadImage(AfxGetInstanceHandle(), TEXT("VIEW_BACK"));
	SetWindowPos(NULL, 0, 0, m_pImageBack.GetWidth(), m_pImageBack.GetHeight(), SWP_NOMOVE);

	//����͸������
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//::SetLayeredWindowAttributes(GetSafeHwnd(), 0, 255, LWA_ALPHA);    // ���ò�͸��
	UpdateLayered();

	//��ȡ�����ļ�--û������Ĭ��Ϊ
	m_strProvince	= AfxGetApp()->GetProfileString(TEXT("weather"), TEXT("Province"), TEXT("�㶫ʡ"));
	m_strCity		= AfxGetApp()->GetProfileString(TEXT("weather"), TEXT("City"), TEXT("������"));
	m_strDistrict	= AfxGetApp()->GetProfileString(TEXT("weather"), TEXT("District"), TEXT("�����"));

	//�����߳��������ݲ���ѯ����
	StartThread();

	InitPosition();
	return TRUE;  
}



void CAngoWeatherDlg::OnPaint()
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
HCURSOR CAngoWeatherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


bool CAngoWeatherDlg::OnEventThreadRun()
{
	bool bRet = false;
	HINSTANCE hInstance = AfxGetInstanceHandle();

	//��������ݺܴ�û�б���ÿ�β�ѯ��ʱ�򶼲���һ������������ֻ��ʼ��һ�ξͺ���
	static bool bInit = false;
	if (!bInit)
	{
		//������Դ
		HRSRC hResource = FindResource(hInstance, TEXT("JSONDATA"), TEXT("JSON"));
		if (hResource == NULL) return false;

		//��ȡ��Դ
		LPVOID pBuffer = LoadResource(hInstance, hResource);

		bRet = m_pParseJson->SetCityJsonData((LPCTSTR)pBuffer);

		bInit = true;
	}

	CString str = m_pParseJson->GetCode(m_strCity, m_strProvince);
	if (str.GetLength() == 0)
	{
		return false;
	}

	CString strURL, StrJson;
	strURL.Format(TEXT("http://weather.gtimg.cn/city/%s.js?ref=qqchannel"), str);

	bRet = m_pParseJson->SetWeatherJsonData(m_HttpWeather.GetHttpFile(strURL));

	//��ȡ����
	CString strData;
	strData.Format(TEXT("WEATHER_%d"), GetValue(TEXT("sk_wt")));
	m_ImageWeather.DestroyImage();
	m_ImageWeather.LoadImage(AfxGetInstanceHandle(), strData);

	m_Weather.m_nHd = GetValue(TEXT("sk_hd"));
	m_Weather.m_nPs = GetValue(TEXT("sk_ps"));
	m_Weather.m_nTp = GetValue(TEXT("sk_tp"));
	m_Weather.m_nVb = GetValue(TEXT("sk_vb"));
	m_Weather.m_nWd = GetValue(TEXT("sk_wd"));
	m_Weather.m_nWp = GetValue(TEXT("sk_wp"));
	m_Weather.m_nWt = GetValue(TEXT("sk_wt"));

	_sntprintf_s(m_Weather.szCity, -1, CountArray(m_Weather.szCity), m_strCity);

	m_bLoadFinished = true;

	UpdateLayered();

	return false;
}


void CAngoWeatherDlg::OnDestroy()
{
	__super::OnDestroy();

	ConcludeThread(0);
}

#define CHECK_IMAGE(p)	if( !p.IsNull() )

void CAngoWeatherDlg::UpdateLayered()
{
	if (GetSafeHwnd() == NULL || !::IsWindow(GetSafeHwnd())) return;
	if (m_pImageBack.IsNull()) return;

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(m_pImageBack.GetWidth(), m_pImageBack.GetHeight(), 32);

	//�滭����
	CImageDC ImageDC(ImageBuffer);
	CDC * pBufferDC = CDC::FromHandle(ImageDC);

	CSize szFont(0, 0);

	//�滭����
	ASSERT(pBufferDC != NULL);
	if (pBufferDC != NULL)
	{
		m_pImageBack.DrawImage(pBufferDC, 0, 0);

		if (!m_ImageWeather.IsNull())
			m_ImageWeather.DrawImage(pBufferDC, 0, 0);

		if (m_bLoadFinished)
		{
			//����
			DrawText(pBufferDC, m_Weather.szCity, 230, 60, DT_LEFT, 34, FW_BOLD);

			CString strTp;
			strTp.Format(TEXT("%d��"), m_Weather.m_nTp);
			DrawText(pBufferDC, strTp, 305, 90);

			DrawText(pBufferDC, weatherTxt[m_Weather.m_nWt], 305, 117);

			CString strWdp;
			strWdp.Format(TEXT("%s(%s)"), windDir[m_Weather.m_nWd], windPower[m_Weather.m_nWp]);
			DrawText(pBufferDC, strWdp, 305, 143);

			CString strPs;
			strPs.Format(TEXT("��ǰ��ѹ:%dhPa"), m_Weather.m_nPs);
			DrawText(pBufferDC, strPs, 305, 168);

			CString strVb;
			strVb.Format(TEXT("�ܼ���:%dKm"), m_Weather.m_nVb);
			DrawText(pBufferDC, strVb, 305, 196);

			CString strHd;
			strHd.Format(TEXT("���ʪ��:%d%%"), m_Weather.m_nHd);
			DrawText(pBufferDC, strHd, 305, 223);
		}
	}

	//��������
	BLENDFUNCTION BlendFunction;
	ZeroMemory(&BlendFunction, sizeof(BlendFunction));

	//���ò���
	BlendFunction.BlendOp = 0;
	BlendFunction.BlendFlags = 0;
	BlendFunction.AlphaFormat = AC_SRC_ALPHA;
	BlendFunction.SourceConstantAlpha = 255;

	//���÷ֲ�
	CPoint ImagePoint(0, 0);
	CClientDC ClientDC(this);
	UpdateLayeredWindow(&ClientDC, NULL, &CSize(m_pImageBack.GetWidth() + szFont.cx, m_pImageBack.GetHeight()), pBufferDC, &ImagePoint, 0, &BlendFunction, 2);
}

void CAngoWeatherDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

	__super::OnLButtonDown(nFlags, point);
}

void CAngoWeatherDlg::DrawText(CDC*pDC, LPCTSTR lpszText, int nPosX, int nPosY, int nFlag, int nSize, int Weight, COLORREF colText)
{
	if (lpszText == NULL) return;

	HDC hDC = pDC->GetSafeHdc();

	CFont font;
	font.CreateFont(nSize, 0, 0, 0, Weight, 0, FALSE, 0, 0, 0, 0, 0, 0, TEXT("΢���ź�"));
	pDC->SelectObject(font);

	pDC->SetTextAlign(DT_CENTER);

	UINT nChar;

	CSize szText = pDC->GetTextExtent(lpszText);

	if ((nFlag & DT_RIGHT))
	{
		for (unsigned n = 0; n < _tcslen(lpszText); n++)
		{
			nChar = *(lpszText + n);

			if (nChar >= 0xa0)
			{
				nPosX -= szText.cy;
			}
			else
			{
				nPosX -= 2;
				nPosX -= szText.cy / 2;
			}

		}
	}

	for (unsigned n = 0; n < _tcslen(lpszText); n++)
	{
		nChar = *(lpszText + n);

#if !defined(_UNICODE)
		if (nChar >= 0xa0)
		{
			nChar = (((nChar << 8) & 0xff00) | (*(lpszText + ++n) & 0x00ff)); //++n
		}
#endif

		GLYPHMETRICS gmm;
		MAT2 mat;
		memset(&mat, 0, sizeof(mat));
		mat.eM11.value = 1;
		mat.eM12.value = 0;
		mat.eM21.value = 0;
		mat.eM22.value = 1;

		DWORD dwSize = GetGlyphOutline(hDC, nChar, GGO_BITMAP, &gmm, 0, 0, &mat);
		if (dwSize == -1)
			return;

		BYTE* pBuffer = new BYTE[dwSize];
		if (GetGlyphOutline(hDC, nChar, GGO_BITMAP, &gmm, dwSize, pBuffer, &mat) == GDI_ERROR)
		{
			delete pBuffer;
			return;
		}

		unsigned char *pData = (unsigned char*)pBuffer;
		unsigned int cnt = gmm.gmBlackBoxX / szText.cy + (gmm.gmCellIncX % szText.cy ? 1 : 0);
		cnt *= 4;

		int nCharPosY = nPosY;

		//˫�ֽ�
		if (nChar < 0xa0)
		{
			nPosX += 2;
			nCharPosY += 1;
		}

		for (unsigned int row = 0; row < gmm.gmBlackBoxY; row++)
		{
			for (unsigned int col = 0; col < cnt; col++)
			{
				for (unsigned int i = 0; i < 8; i++)
				{
					if ((*pData) & (0x80 >> i))
					{
						pDC->SetPixel(nPosX + (col * 8 + i), nCharPosY + row, colText);
					}
				}
				pData++;
			}
		}

		if (nChar >= 0xa0)
		{
			nPosX += szText.cy;
		}
		else
		{
			nPosX += szText.cy / 2;
		}

		SafeDeleteArray(pBuffer);
	}
}

int CAngoWeatherDlg::GetValue(CString StrData)
{
	if (m_pParseJson == NULL) return -1;

	m_pParseJson->GetData(StrData);

	return _tcstol(StrData, 0, 10);
}

void CAngoWeatherDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu* pmenu = m_popMenu.GetSubMenu(0);		//�����Ĳ˵�ʵ������IDR_MENU_POPUP�˵���ĳ����Ӳ˵��������ǵ�һ��
	CPoint pos;
	GetCursorPos(&pos);							//�����˵���λ�ã�����������ĵ�ǰλ��
	//��ʾ�ò˵�����һ������������ֵ�ֱ��ʾ�������ұ���ʾ����Ӧ����һ�
	pmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);

	__super::OnRButtonDown(nFlags, point);
}



void CAngoWeatherDlg::OnClose()
{
	if (m_hOnlyMutex)
	{
		CloseHandle(m_hOnlyMutex);
		m_hOnlyMutex = NULL;
	}

	__super::OnClose();
}


//����windows����������F1����
void CAngoWeatherDlg::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	return;
	//CDialog::WinHelp(dwData, nCmd);
}

void CAngoWeatherDlg::OnOK()
{
}

void CAngoWeatherDlg::OnCancel()
{
	OnClose();
	CDialogEx::OnCancel();
}


LRESULT CAngoWeatherDlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
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

void CAngoWeatherDlg::OnMenuShow()
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


void CAngoWeatherDlg::OnMenuHide()
{
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


void CAngoWeatherDlg::OnMenuUp()
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


void CAngoWeatherDlg::OnMenuDown()
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


void CAngoWeatherDlg::OnMenuCityOpt()
{
	if (m_OptionDlg.GetSafeHwnd() == NULL)
	{
		COptionDlg OptionDlg;

		if (OptionDlg.DoModal() == IDOK)
		{

			m_strCity		= OptionDlg.m_strCity;
			m_strProvince	= OptionDlg.m_strProvince;
			m_strDistrict	= OptionDlg.m_strDistrict;

			//д�������ļ�
			AfxGetApp()->WriteProfileString(TEXT("weather"), TEXT("Province"), m_strProvince);
			AfxGetApp()->WriteProfileString(TEXT("weather"), TEXT("City"), m_strCity);
			AfxGetApp()->WriteProfileString(TEXT("weather"), TEXT("District"), m_strDistrict);
			if (IsRuning()) ConcludeThread(0);

			StartThread();
		}
	}
}


void CAngoWeatherDlg::OnMenuPreview()
{
	// TODO:  �ڴ���������������
}


void CAngoWeatherDlg::OnMenuAngo()
{
	CString strPath = CUtils::GetAppDir();
	strPath += _T("\\Ango.exe");
	//���API����ʾ�Ƿ��Թ���Ա�������
	ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL);
}


void CAngoWeatherDlg::OnMenuConfig()
{
	// TODO:  �ڴ���������������
}


void CAngoWeatherDlg::OnMenuExit()
{
	OnClose();
	CDialogEx::OnCancel();
}


int CAngoWeatherDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	//֪ͨ����ͼ�꣬����ͼ��
	m_trayIcon.m_pCwnd = this;
	m_trayIcon.m_dwIconId = IDR_MAINFRAME;
	m_trayIcon.m_strTips = _T("����");
	m_trayIcon.InitTrayIcon();

	return 0;
}


void CAngoWeatherDlg::InitPosition()
{
	//����λ��
	CRect cr;
	GetClientRect(&cr);//��ȡ�Ի���ͻ������С
	ClientToScreen(&cr);//ת��ΪӫĻ����
	int x = GetSystemMetrics(SM_CXSCREEN);//��ȡӫĻ����Ŀ�ȣ���λΪ����
	int y = GetSystemMetrics(SM_CYSCREEN);//��ȡӫĻ����ĸ߶ȣ���λΪ����
	//MoveWindow((x-cr.Width() *2)/2 ,cr.top,cr.Width() *2,cr.Height() *2);//���Ͻ�
	//MoveWindow(x - cr.Width(), cr.Height(), cr.Width(), cr.Height());
	MoveWindow(x - cr.Width(), 0, 0, 0);
}