
// AngoTimeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AngoTime.h"
#include "AngoTimeDlg.h"
#include "afxdialogex.h"
#include "MsgBoxEx.h"

#define pi 3.1415926535897932384626433832795028841971693993751058209

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAngoTimeDlg �Ի���



CAngoTimeDlg::CAngoTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngoTimeDlg::IDD, pParent)
	, m_uTimer(0)
	, point1(0)
	, point2(0)
	, ss(0)
	, m(0)
	, h(0)
{
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
END_MESSAGE_MAP()


// CAngoTimeDlg ��Ϣ�������

BOOL CAngoTimeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
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
		AngoMessageBox(_T("�����Ѿ�������"));
		CDialog::OnCancel();
	}

	InitClock();


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
		OnTimer(m_uTimer);
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
	CString strMsg = L"�س���";
	AngoMessageBox(strMsg);

}

void CAngoTimeDlg::OnCancel()
{
	CString strMsg;
	strMsg = "�Ƿ��˳�����";
	if (AngoMessageBox(strMsg, L"�˳�", MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}
	CDialog::OnCancel();
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

void CAngoTimeDlg::OnMenuExit()
{
	OnCancel();
}

//-------------------------------------------------------------------------------------------------------------------------
void CAngoTimeDlg::InitClock()
{
	//���ÿ���ʱ���߶��Ĵ�����Ϊÿ��һ�Σ�������WM_TIMER��Ϣ��Ƶ����ÿ��һ�Ρ�
	m_uTimer = this->SetTimer(1, 1000, NULL);

	/////////////////////////////////////////////////////////////
	//��Բ�ζԻ���
	CRgn  rgn;
	CRect  rc;
	GetClientRect(&rc);
	rgn.CreateEllipticRgn(rc.left, rc.top, rc.right, rc.bottom);
	SetWindowRgn(rgn, TRUE);
	rgn.DeleteObject();
	//////////////////////////////////////////////////////////////
	//��ʱ�ӱ���ͼƬ��Բ�ζԻ��򱳾�
	CBitmap   bm;
	bm.LoadBitmap(IDB_BMP_CLOCK);   //   IDB_BITMAP1ΪBITMAP��ԴID,����ָ��bitmapͼƬ��·��   
	m_cBrush.CreatePatternBrush(&bm);
}

HBRUSH CAngoTimeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return m_cBrush;
}

void CAngoTimeDlg::OnTimer(UINT_PTR nIDEvent)//����ʱ���߶�
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//�жϴ��ݹ�����ʱ�Ӵ������Ƿ����Լ������ʱ�Ӵ�����
	if(nIDEvent == m_uTimer) 
	{
		//��õ�ǰϵͳʱ�䡣
		CTime time = CTime::GetCurrentTime();
		int C;							//���ڼ�����ɫ
		CPen *PenOld,PenNew;
		CBrush *BrushOld,BrushNew;
		CClientDC dc(this);
		int S=time.GetSecond();
		float M=float(time.GetMinute()+S/60.0);
		float H=float(time.GetHour()+M/60.0);
		if(H>12)
			H=H-12;
		H=H*5;
		point1.x=103;
		point1.y=102;

//		OnPaint();
		//����Ϊ��ʱ����������
		//����Ϊ��ÿ����ǰ�ñ���ɫ��ȥ��һ�λ����루���ڱ���ɫ���䣬���Լ����˼��㣩
//////////////////////////////////////////////	
		if(h<5)
			C=232;
		else if(h<10)
			C=235;
		else if(h<15)
			C=242;
		else if(h<20)
			C=247;
		else if(h<30)
			C=248;
		else if(h<35)
			C=244;
		else if(h<40)
			C=242;
		else if(h<47)
			C=235;
		else
			C=230;

		PenNew.CreatePen(PS_SOLID,4,RGB(C,C,C));
		BrushNew.CreateSolidBrush(RGB(C,C,C));
		BrushOld=dc.SelectObject(&BrushNew);
		PenOld=dc.SelectObject(&PenNew);
		point2.x=103+35*sin(h*pi/30);
		point2.y=102-35*cos(h*pi/30);
		dc.MoveTo(point1);
		dc.LineTo(point2);


		PenNew.DeleteObject();
		PenNew.CreatePen(PS_SOLID,4,RGB(0,0,0));
		PenOld=dc.SelectObject(&PenNew);
		BrushNew.DeleteObject();
		BrushNew.CreateSolidBrush(RGB(0,0,0));
		BrushOld=dc.SelectObject(&BrushNew);
		point2.x=103+35*sin(H*pi/30);
		point2.y=102-35*cos(H*pi/30);
		dc.MoveTo(point1);
		dc.LineTo(point2);
///////////////////////////////////////////////
		if(m<5)
			C=232;
		else if(m<10)
			C=235;
		else if(m<15)
			C=242;
		else if(m<20)
			C=247;
		else if(m<30)
			C=248;
		else if(m<35)
			C=244;
		else if(m<40)
			C=242;
		else if(m<47)
			C=235;
		else
			C=230;

		BrushNew.DeleteObject();
		BrushNew.CreateSolidBrush(RGB(C,C,C));
		BrushOld=dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_SOLID,4,RGB(C,C,C));
		PenOld=dc.SelectObject(&PenNew);
		point2.x=103+55*sin(m*pi/30);
		point2.y=102-55*cos(m*pi/30);
		dc.MoveTo(point1);
		dc.LineTo(point2);



		BrushNew.DeleteObject();
		BrushNew.CreateSolidBrush(RGB(0,0,0));
		BrushOld=dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_SOLID,4,RGB(0,0,0));
		PenOld=dc.SelectObject(&PenNew);
		point2.x=103+55*sin(M*pi/30);
		point2.y=102-55*cos(M*pi/30);
		dc.MoveTo(point1);
		dc.LineTo(point2);
		
////////////////////////////////////////////	������Ķ���	
		ss=(ss+30)%60;
		S=(S+30)%60;
		if(ss<5)
			C=232;
		else if(ss<10)
			C=235;
		else if(ss<15)
			C=242;
		else if(ss<20)
			C=247;
		else if(ss<30)
			C=248;
		else if(ss<35)
			C=244;
		else if(ss<40)
			C=242;
		else if(ss<47)
			C=235;
		else
			C=230;
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT,2,RGB(C,C,C));
		PenOld=dc.SelectObject(&PenNew);
		point2.x=103+10*sin(ss*pi/30);
		point2.y=102-10*cos(ss*pi/30);
		dc.MoveTo(point1);
		dc.LineTo(point2);


		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT,2,RGB(255,0,0));
		PenOld=dc.SelectObject(&PenNew);
		point2.x=103+10*sin(S*pi/30);
		point2.y=102-10*cos(S*pi/30);
		dc.MoveTo(point1);
		dc.LineTo(point2);


///////////////////////������ĳ���
		ss=(ss+30)%60;
		S=(S+30)%60;
		if(ss<5)
			C=232;
		else if(ss<10)
			C=235;
		else if(ss<15)
			C=242;
		else if(ss<20)
			C=247;
		else if(ss<30)
			C=248;
		else if(ss<35)
			C=244;
		else if(ss<40)
			C=242;
		else if(ss<47)
			C=235;
		else
			C=230;
//		BrushNew.DeleteObject();
//		BrushNew.CreateSolidBrush(RGB(C,C,C));
//		BrushOld=dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT,2,RGB(C,C,C));
		PenOld=dc.SelectObject(&PenNew);
		point2.x=103+63*sin(ss*pi/30);
		point2.y=102-63*cos(ss*pi/30);
		dc.MoveTo(point1);
		dc.LineTo(point2);


//		BrushNew.DeleteObject();
//		BrushNew.CreateSolidBrush(RGB(255,0,0));
//		BrushOld=dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT,2,RGB(255,0,0));
		PenOld=dc.SelectObject(&PenNew);
		point2.x=103+63*sin(S*pi/30);
		point2.y=102-63*cos(S*pi/30);
		dc.MoveTo(point1);
		dc.LineTo(point2);



/*//////////////////////////////////////////////////////////////////
		CBrush m_brush_trad_s;
		CBitmap   bm_trad_s;   
		bm_trad_s.LoadBitmap(IDB_Trad_s);   //   IDB_Trad_sΪBITMAP��ԴID,����ָ��bitmapͼƬ��·��   
		m_brush_trad_s.CreatePatternBrush(&bm_trad_s); 
		BrushOld=dc.SelectObject(&m_brush_trad_s);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_NULL,1,RGB(255,255,255));//(PS_DASHDOTDOT,2,RGB(255,0,0));
		PenOld=dc.SelectObject(&PenNew);
		point2.x=103+63*sin(S*pi/30);
		point2.y=102-63*cos(S*pi/30);
		dc.Rectangle(94,40,107,169);
*///////////////////////////////////////////////////////////////////
		h=H;m=M;ss=S;
//////////////////////////////////////////////////////////////////
		dc.SetPixel(point1,RGB(0,0,0));
		dc.SetPixel(point1.x+1,point1.y,RGB(0,0,0));
		dc.SetPixel(point1.x,point1.y+1,RGB(0,0,0));
		dc.SetPixel(point1.x+1,point1.y+1,RGB(0,0,0));
		dc.SetPixel(point1.x-1,point1.y,RGB(0,0,0));
		dc.SetPixel(point1.x,point1.y-1,RGB(0,0,0));
		dc.SetPixel(point1.x-1,point1.y-1,RGB(0,0,0));





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
		this->UpdateData(false);
	}
	CDialog::OnTimer(nIDEvent);
}


//-------------------------------------------------------------------------------------------------------------------------