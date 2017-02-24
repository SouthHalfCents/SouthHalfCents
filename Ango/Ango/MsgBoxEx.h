#pragma once
#include "afxwin.h"


// CMsgBoxEx 对话框
// 替换MessageBox

class CMsgBoxEx : public CDialogEx
{
	DECLARE_DYNAMIC(CMsgBoxEx)

public:
	CMsgBoxEx(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMsgBoxEx();

// 对话框数据
	enum { IDD = IDD_DLG_MSGBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL			OnInitDialog();
	DECLARE_MESSAGE_MAP()

	CBrush					m_MyBrush;
	HBRUSH					m_MyHBrush;
	afx_msg HBRUSH			OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL			OnEraseBkgnd(CDC* pDC);

public:
	// nType只支持
	/*
	#define MB_OK                       0x00000000L
	#define MB_OKCANCEL                 0x00000001L
	#define MB_ABORTRETRYIGNORE         0x00000002L
	#define MB_YESNOCANCEL              0x00000003L
	#define MB_YESNO                    0x00000004L
	#define MB_RETRYCANCEL              0x00000005L
	#define MB_CANCELTRYCONTINUE        0x00000006L
	*/

	//返回值
	/*
	#define IDOK                1
	#define IDCANCEL            2
	#define IDABORT             3
	#define IDRETRY             4
	#define IDIGNORE            5
	#define IDYES               6
	#define IDNO                7
	#define IDCLOSE				8
	#define IDHELP				9
	#define IDTRYAGAIN			10
	#define IDCONTINUE			11
	*/

	CButton						m_btnYes;
	CButton						m_btnNo;
	CButton						m_btnCancle;
	CEdit						m_edtMsg;

	CString						m_strYes;
	CString						m_strNo;
	CString						m_strCancle;
	CString						m_strMsg;
	CString						m_strTitle;

	UINT						m_nType;

	afx_msg void				OnBnClickedBtnYes();
	afx_msg void				OnBnClickedBtnNo();
	afx_msg void				OnBnClickedBtnCancle();
	void						InitMsgBox(CString strMsg, CString strTitle=L"", UINT nType=MB_OK);
	void						SetRayout();
	

	
};


class AngoMsgBox
{
public:
	AngoMsgBox();
	~AngoMsgBox();

	static int				MessageBox(CString strMsg, CString strTitle=L"", UINT nType=MB_OK);
};