
// AngoDlg.h : ͷ�ļ�
//

#pragma once


// CAngoDlg �Ի���
class CAngoDlg : public CDialogEx
{
// ����
public:
	CAngoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ANGO_MAIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON					m_hIcon;

	//// 
	virtual BOOL			OnInitDialog();
	afx_msg int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void			OnPaint();
	afx_msg HCURSOR			OnQueryDragIcon();
	afx_msg void			OnDestroy();

	///// ��Ӧ�˵�
	afx_msg void			OnShowDlg();
	afx_msg void			OnAboutDlg();
	afx_msg void			OnExitDlg();
	afx_msg void			OnConfig();
	
	//// ��Ӧ����
	virtual void			OnOK();							//����
	virtual void			OnCancel();						//����
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	//// �Զ�����Ϣ
	afx_msg LRESULT			OnNotifyIcon(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()


	HRGN					BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance=NULL);	//����BMP����ʾ����
	BOOL					OnReInitDialog();				//�������ػ�Ի������


public:
	BOOL					m_bMin;				//�Ƿ���С��״̬
	NOTIFYICONDATA			m_ntIcon;			//���֪ͨ����ͼ��

	
};
