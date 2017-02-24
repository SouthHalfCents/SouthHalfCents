
// AngoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Ango.h"
#include "AngoDlg.h"
#include "afxdialogex.h"

#include "string"
#include "Database.h"
#include "BaseDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using std::string;

// CAngoDlg 对话框



CAngoDlg::CAngoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngoDlg::IDD, pParent)
{
	m_bMin	=	FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	ON_WM_DESTROY()
	ON_COMMAND(ID_SHOW_DLG, &CAngoDlg::OnShowDlg)
	ON_COMMAND(ID_ABOUT_DLG, &CAngoDlg::OnAboutDlg)
	ON_COMMAND(ID_EXIT_DLG, &CAngoDlg::OnExitDlg)
	ON_COMMAND(ID_CONFIG, &CAngoDlg::OnConfig)
END_MESSAGE_MAP()


// CAngoDlg 消息处理程序
//-------------------------------------------------------------------------------------------------------------------------
BOOL CAngoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	

	// 唯一实例
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, L"Ango");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 如果程序已经存在并且正在运行 
		// 如果已有互斥量存在则释放句柄并复位互斥量，退出程序
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		MessageBox(L"程序已经在运行");
		CDialog::OnCancel();
	}


	//隐藏任务栏
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);


	CBitmap bmp;
	if( g_nCurrentBmpID == 1 )
	{
		if (bmp.LoadBitmap(IDB_BITMAP_MAIN_1))
		{
			HRGN rgn;
			rgn = BitmapToRegion((HBITMAP)bmp, RGB(0, 0, 0));
			SetWindowRgn(rgn, TRUE);
			bmp.DeleteObject();
		}
	}
	else if( g_nCurrentBmpID == 2 )
	{
		if (bmp.LoadBitmap(IDB_BITMAP_MAIN_2))
		{
			HRGN rgn;
			rgn = BitmapToRegion((HBITMAP)bmp, RGB(0, 0, 0));
			SetWindowRgn(rgn, TRUE);
			bmp.DeleteObject();
		}
	}
	else
	{
		if (bmp.LoadBitmap(IDB_BITMAP_MAIN_2))
		{
			HRGN rgn;
			rgn = BitmapToRegion((HBITMAP)bmp, RGB(0, 0, 0));
			SetWindowRgn(rgn, TRUE);
			bmp.DeleteObject();
		}
	}



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAngoDlg::OnPaint()
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
		//设置界面
		CClientDC dc(this);
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		CBitmap bmp;

		if(g_nCurrentBmpID == 1)
		{
			bmp.LoadBitmap(IDB_BITMAP_MAIN_1);
		}
		else if(g_nCurrentBmpID == 2)
		{
			bmp.LoadBitmap(IDB_BITMAP_MAIN_2);
		}
		else
		{
			bmp.LoadBitmap(IDB_BITMAP_MAIN_1);
		}

		CBitmap *pOldBmp;
		pOldBmp = memDC.SelectObject(&bmp);
		BITMAP bm;
		bmp.GetBitmap(&bm);
		dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOldBmp);
		bmp.DeleteObject();

		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CAngoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CAngoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ntIcon.cbSize = sizeof(NOTIFYICONDATA);							//该结构体变量的大小
	m_ntIcon.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);				//图标，通过资源ID得到
	m_ntIcon.uID = IDR_MAINFRAME;
	m_ntIcon.hWnd = this->m_hWnd;										//接收托盘图标通知消息的窗口句柄
	WCHAR szTips[128] = L"Ango";										//鼠标设上面时显示的提示
	wcscpy_s(m_ntIcon.szTip, 128, szTips);
	m_ntIcon.uCallbackMessage = MAIN_WM_NOTIFYICON;						//应用程序定义的消息ID号
	m_ntIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;					//图标的属性：设置成员uCallbackMessage、hIcon、szTip有效
	::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);								//在系统通知区域增加这个图标

	return 0;
}
void CAngoDlg::OnDestroy()
{

	//删除该通知状态图标
	::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);

	CDialogEx::OnDestroy();
}
//-------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------
//拖动无标题对话框窗口
void CAngoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

//拖动无标题对话框窗口
void CAngoDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU1);				//IDR_MENU_POPUP是在ResourceView中创建并编辑的一个菜单
	CMenu* pmenu = popMenu.GetSubMenu(0);		//弹出的菜单实际上是IDR_MENU_POPUP菜单的某项的子菜单，这里是第一项
	CPoint pos;
	GetCursorPos(&pos);							//弹出菜单的位置，这里就是鼠标的当前位置
	//显示该菜单，第一个参数的两个值分别表示在鼠标的右边显示、响应鼠标右击
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
		strMsg = "回车键";
	}
	break;

	case 16:
	{
		g_nCurrentBmpID = (g_nCurrentBmpID % 2) + 1;
		OnReInitDialog();
		OnPaint();
		//MessageBox(L"Shift键");
	}
	break;

	case 27:
	{
		strMsg = "ESC";
		MessageBox(strMsg);
		exit(0);
	}
	break;

	case 112:
	{

		//AfxMessageBox("F1键");
	}
	break;

	case 113:
	{
// 		CBaseDialog baseDlg;
// 		baseDlg.DoModal();
		//AfxMessageBox("F2键");
	}
	break;

	case 114:
	{
		//AfxMessageBox("F3键");
	}
	break;

	case 115:
	{
		//AfxMessageBox("F4键");
	}
	break;

	case 116:
	{
		//AfxMessageBox("F5键");
	}
	break;

	case 117:
	{
		//创建非模式对话框，不过要创建的是类成员  在C龙行天下Dlg类里声明  CF1Dlg F1Dlg;
		//if(!IsWindow(m_F6Dlg.m_hWnd))
		//{
		//是否关联了对话框，关闭非模式对话框时，只是隐藏，没有销毁对话框资源
		//m_F6Dlg.Create(IDD_F6_DIALOG,this);
		//}
		//m_F6Dlg.ShowWindow(SW_SHOW);

		//AfxMessageBox("F6键");
	}
	break;
	default:
		strMsg = "其他键";
	}

	if (strMsg.GetLength())
	{
		MessageBox(strMsg);
	}


	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CAngoDlg::OnOK()
{
	CString strMsg = L"回车键";
	MessageBox(strMsg);

}

void CAngoDlg::OnCancel()
{
	CDialog::OnCancel();
}

//-------------------------------------------------------------------------------------------------------------------------
LRESULT CAngoDlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
{
	if (lparam == WM_LBUTTONDOWN)
	{
		//恢复窗口或者最小化
		if (m_bMin == TRUE)
		{
			AfxGetMainWnd()->ShowWindow(SW_SHOW);
			AfxGetMainWnd()->ShowWindow(SW_RESTORE);
			//这里貌似只有写这样两句才能保证恢复窗口后，该窗口处于活动状态（在最前面）
			m_bMin = FALSE;
		}
		else
		{
			AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
			m_bMin = TRUE;
		}
	}
	else if (lparam == WM_RBUTTONDOWN)
	{
		//弹出右键菜单
		CMenu popMenu;
		popMenu.LoadMenu(IDR_MENU1);				//IDR_MENU_POPUP是在ResourceView中创建并编辑的一个菜单
		CMenu* pmenu = popMenu.GetSubMenu(0);		//弹出的菜单实际上是IDR_MENU_POPUP菜单的某项的子菜单，这里是第一项
		CPoint pos;
		GetCursorPos(&pos);							//弹出菜单的位置，这里就是鼠标的当前位置
		//显示该菜单，第一个参数的两个值分别表示在鼠标的右边显示、响应鼠标右击
		pmenu->TrackPopupMenu( TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);
		
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------------

void CAngoDlg::OnShowDlg()
{
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);
	//这里貌似只有写这样两句才能保证恢复窗口后，该窗口处于活动状态（在最前面）
	m_bMin = FALSE;
}

void CAngoDlg::OnAboutDlg()
{
	CString strMsg;
	strMsg = "说明";
	MessageBox(strMsg);
}

void CAngoDlg::OnExitDlg()
{
	CDialog::OnCancel();
}


void CAngoDlg::OnConfig()
{
	// TODO:  在此添加命令处理程序代码
}
//-------------------------------------------------------------------------------------------------------------------------
//运行中重绘对话框界面
BOOL CAngoDlg::OnReInitDialog()
{


	CBitmap bmp;
	if (g_nCurrentBmpID == 1)
	{
		if (bmp.LoadBitmap(IDB_BITMAP_MAIN_1))
		{
			HRGN rgn;
			rgn = BitmapToRegion((HBITMAP)bmp, RGB(0, 0, 0));
			SetWindowRgn(rgn, TRUE);
			bmp.DeleteObject();
		}
	}
	else if (g_nCurrentBmpID == 2)
	{
		if (bmp.LoadBitmap(IDB_BITMAP_MAIN_2))
		{
			HRGN rgn;
			rgn = BitmapToRegion((HBITMAP)bmp, RGB(0, 0, 0));
			SetWindowRgn(rgn, TRUE);
			bmp.DeleteObject();
		}
	}
	else
	{
		if (bmp.LoadBitmap(IDB_BITMAP_MAIN_2))
		{
			HRGN rgn;
			rgn = BitmapToRegion((HBITMAP)bmp, RGB(0, 0, 0));
			SetWindowRgn(rgn, TRUE);
			bmp.DeleteObject();
		}
	}

	return TRUE;
}


HRGN CAngoDlg::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
{
	HRGN hRgn = NULL;

	if (hBmp)
	{
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);

			//创建一个32位色的位图，并选进内存设备环境
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {
				sizeof(BITMAPINFOHEADER),		// biSize 
				bm.bmWidth,					// biWidth; 
				bm.bmHeight,				// biHeight; 
				1,							// biPlanes; 
				32,							// biBitCount 
				BI_RGB,						// biCompression; 
				0,							// biSizeImage; 
				0,							// biXPelsPerMeter; 
				0,							// biYPelsPerMeter; 
				0,							// biClrUsed; 
				0							// biClrImportant; 
			};
			VOID * pbits32;
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y + 1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y + 1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y + 1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}
				DeleteObject(SelectObject(hMemDC, holdBmp));
			}
			DeleteDC(hMemDC);
		}
	}
	return hRgn;
}
//-------------------------------------------------------------------------------------------------------------------------