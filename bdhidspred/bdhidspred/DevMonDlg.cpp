// DevMonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DevMonDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

#include <DBT.h>

#define	BDHIDS_DISKCHANGED_EVENT	 "Global\\BdhidsDiskChangedEvent"
#define	BDHIDS_DEVCHANGED_EVENT	     "Global\\BdhidsDevChangedEvent"


 
CDevMonDlg::CDevMonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDevMonDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDevMonDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevChangedEvent=m_hDiskChangedEvent=NULL;
	m_hDevNotify=NULL;
}

void CDevMonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDevMonDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDevMonDlg, CDialog)
	//{{AFX_MSG_MAP(CDevMonDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDevMonDlg message handlers

BOOL CDevMonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

 
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = 
        sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;


    m_hDevNotify = RegisterDeviceNotification(m_hWnd, 
        &NotificationFilter,
        DEVICE_NOTIFY_WINDOW_HANDLE |  0x00000004);

    if(m_hDevNotify) 
    {
		OutputDebugString("RegisterDeviceNotification success!\n");	     
    }

	SECURITY_ATTRIBUTES		sa;
	SECURITY_DESCRIPTOR		sd;

	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	m_hDevChangedEvent=CreateEvent(&sa,TRUE,FALSE,BDHIDS_DEVCHANGED_EVENT);
	if(m_hDevChangedEvent == NULL)
	{
		return FALSE;
	}
	ResetEvent(m_hDevChangedEvent);


	m_hDiskChangedEvent=CreateEvent(&sa,TRUE,FALSE,BDHIDS_DISKCHANGED_EVENT);
	if(m_hDiskChangedEvent == NULL)
	{
		return FALSE;
	}
	ResetEvent(m_hDiskChangedEvent);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDevMonDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_hDevNotify!=NULL)
	{
		UnregisterDeviceNotification(m_hDevNotify);
		m_hDevNotify=NULL;
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDevMonDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDevMonDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

char FirstDriveFromMask (ULONG unitmask)
{
   char i;

   for (i = 0; i < 26; ++i)
   {
      if (unitmask & 0x1)
         break;
      unitmask = unitmask >> 1;
   }

   return (i + 'A');
}

BOOL CDevMonDlg::OnDeviceChange( UINT nEventType, DWORD dwData )
{
	PDEV_BROADCAST_HDR			pdev;


	if(nEventType == DBT_DEVICEARRIVAL)
	{
		OutputDebugString("bdhidspred DBT_DEVICEARRIVAL\n");
		pdev = (PDEV_BROADCAST_HDR)dwData;

		if(m_hDevChangedEvent!=NULL) ::SetEvent(m_hDevChangedEvent);
		if(pdev->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			    OutputDebugString("bdhidspred DBT_DEVTYP_VOLUME\n");
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)pdev;
				if (lpdbv -> dbcv_flags & DBTF_MEDIA)
				{
					char szMsg[MAX_PATH];
				   _snprintf(szMsg,MAX_PATH, "Drive %c: Media has arrived.\n",
							 FirstDriveFromMask(lpdbv ->dbcv_unitmask));

				   OutputDebugString(szMsg);
				}
				if(m_hDiskChangedEvent!=NULL) ::SetEvent(m_hDiskChangedEvent);
		}
	}
	else if(nEventType == DBT_DEVICEREMOVECOMPLETE)
	{
		pdev = (PDEV_BROADCAST_HDR)dwData;

		OutputDebugString("bdhidspred DBT_DEVICEREMOVECOMPLETE\n");
		if(pdev->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			OutputDebugString("bdhidspred DBT_DEVICEREMOVECOMPLETE=>DBT_DEVTYP_VOLUME\n");
			PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)pdev;

            if (lpdbv -> dbcv_flags & DBTF_MEDIA)
            {
				char szMsg[MAX_PATH];
				_snprintf(szMsg,MAX_PATH, "Drive %c: Media  was removed..\n",
					FirstDriveFromMask(lpdbv ->dbcv_unitmask));
				OutputDebugString(szMsg);
            }

		}
	}

	return true;
}

