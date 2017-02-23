
// AngoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Ango.h"
#include "AngoDlg.h"
#include "afxdialogex.h"

#include "Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAngoDlg �Ի���



CAngoDlg::CAngoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngoDlg::IDD, pParent)
{
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
END_MESSAGE_MAP()


// CAngoDlg ��Ϣ�������

BOOL CAngoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	

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


	//����������
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	//����֪ͨ����ͼ��


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
		//���ý���
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CAngoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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

			//����һ��32λɫ��λͼ����ѡ���ڴ��豸����
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
			HBITMAP hbm32 = CreateDIBSection(hMemDC,(BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
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
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
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

//�϶��ޱ���Ի��򴰿�
void CAngoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

//�϶��ޱ���Ի��򴰿�
void CAngoDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	MessageBox(L"�Ҽ�");
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
		g_nCurrentBmpID = (g_nCurrentBmpID % 2) + 1;
		OnReInitDialog();
		OnPaint();
		//MessageBox(L"Shift��");
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
		//AfxMessageBox("F1��");
	}
	break;

	case 113:
	{
		//AfxMessageBox("F2��");
	}
	break;

	case 114:
	{
		//AfxMessageBox("F3��");
	}
	break;

	case 115:
	{
		//AfxMessageBox("F4��");
	}
	break;

	case 116:
	{
		//AfxMessageBox("F5��");
	}
	break;

	case 117:
	{
		//������ģʽ�Ի��򣬲���Ҫ�����������Ա  ��C��������Dlg��������  CF1Dlg F1Dlg;
		//if(!IsWindow(m_F6Dlg.m_hWnd))
		//{
		//�Ƿ�����˶Ի��򣬹رշ�ģʽ�Ի���ʱ��ֻ�����أ�û�����ٶԻ�����Դ
		//m_F6Dlg.Create(IDD_F6_DIALOG,this);
		//}
		//m_F6Dlg.ShowWindow(SW_SHOW);

		//AfxMessageBox("F6��");
	}
	break;
	default:
		strMsg = "������";
	}

	if (strMsg.GetLength())
	{
		MessageBox(strMsg);
	}


	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

//�������ػ�Ի������
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

void CAngoDlg::OnOK()
{
	CString strMsg = L"�س���";
	MessageBox(strMsg);

}

void CAngoDlg::OnCancel()
{
	//CString strMsg = L"ESC��";
	//MessageBox(strMsg);
	CDialog::OnCancel();
}