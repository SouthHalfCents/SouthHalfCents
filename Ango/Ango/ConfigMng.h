#pragma once

//// ���ù���



// CConfigMng �Ի���

class CConfigMng : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigMng)

public:
	CConfigMng(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigMng();

// �Ի�������
	enum { IDD = IDD_CFGMNG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL			OnInitDialog();
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	CBrush					m_MyBrush;
	HBRUSH					m_MyHBrush;
	afx_msg HBRUSH			OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL			OnEraseBkgnd(CDC* pDC);

	void					SetDlgPos();


public:
	
};
