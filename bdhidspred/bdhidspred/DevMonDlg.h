// DevMonDlg.h : header file
//

#if !defined(AFX_DEVMONDLG_H__36EAE2A5_6D0C_48F6_B40C_69D954B671F7__INCLUDED_)
#define AFX_DEVMONDLG_H__36EAE2A5_6D0C_48F6_B40C_69D954B671F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDevMonDlg dialog
#include "resource.h"


class CDevMonDlg : public CDialog
{
// Construction
public:
	CDevMonDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDevMonDlg)
	enum { IDD = IDD_DEVMON_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDevMonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	HDEVNOTIFY  m_hDevNotify; 
	HANDLE      m_hDevChangedEvent,m_hDiskChangedEvent;
	
	// Generated message map functions
	//{{AFX_MSG(CDevMonDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVMONDLG_H__36EAE2A5_6D0C_48F6_B40C_69D954B671F7__INCLUDED_)
